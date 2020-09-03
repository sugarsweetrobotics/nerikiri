
#include <iostream>
#include <memory>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"
#include "nerikiri/operationfactory.h"
#include "nerikiri/connectionbuilder.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;


bool operationIsCalled = false;

class OneShotEC : public ExecutionContext {
public:
    OneShotEC(const Value& info) : ExecutionContext(info) {}

public:
    virtual bool onStarted() override {
        svc();
        return true;
    }
};

auto opf1 = std::shared_ptr<OperationFactory>( new OperationFactory{
  { {"typeName", "add"},
    {"defaultArg", {
      {"arg01", 0},
      {"arg02", 0}
    }}
  },
  [](const Value& arg) -> Value {
    operationIsCalled = true;
    return Value(arg.at("arg01").intValue() + arg.at("arg02").intValue());
  }
});

auto opf2 = std::shared_ptr<OperationFactory>( new OperationFactory{
  { {"typeName", "inc"},
    {"defaultArg", {
      {"arg01", 0}
    }}
  },
  [](const Value& arg) -> Value {
    operationIsCalled = true;
    return Value(arg.at("arg01").intValue()+1);
  }
});

auto opf3 = std::shared_ptr<OperationFactory>( new OperationFactory{
  { {"typeName", "zero"},
    {"defaultArg", {}}
  },
  [](const Value& arg) -> Value {
    operationIsCalled = true;
    return Value(0);
  }
});


SCENARIO( "FSM test", "[ec]" ) {
  GIVEN("FSM basic behavior") {
    const std::string jsonStr = R"({
      "logger": { "logLevel": "OFF" },

      "operations": {
        "precreate": [
          { "typeName": "add", "instanceName": "add0.ope"},
          { "typeName": "zero", "instanceName": "zero0.ope"},
          { "typeName": "inc", "instanceName": "inc0.ope"}
        ]
      },  

      "fsms": {
          "precreate": [
              {
                  "typeName": "GenericFSM",
                  "instanceName": "FSM0.fsm",
                  "states" : [
                    {
                      "name": "created",
                      "transit": ["stopped"]
                    },
                    { 
                      "name": "stopped",
                      "transit": ["running"] 
                    },
                    { 
                      "name": "running",
                      "transit": ["stopped"]
                    }
                  ],
                  "defaultState" : "created"
              }
          ]
      },
      "ecs": {
          "precreate": [
            {
              "typeName": "OneShotEC",
              "instanceName": "OneShotEC0.ec"
            }
          ],
          "bind": {},
          "start": []
      },
      "brokers": {
          "load_paths": ["build/bin", "build/lib", "../../../build/bin"],
          "preload": ["HTTPBroker"],
          "precreate": [
            {
              "typeName": "HTTPBroker",
              "instanceName": "HTTPBroker0.brk",
              "host": "0.0.0.0",
              "port": 8080,
              "baseDir": "${ProjectDirectory}/../systemeditor"
            }
          ]
      }
    })";

    Process p("fsm_test", jsonStr);

    p.loadOperationFactory(opf1);
    p.loadOperationFactory(opf2);
    p.loadOperationFactory(opf3);

    auto ecf1 = std::shared_ptr<ExecutionContextFactory>( static_cast<ExecutionContextFactory*>(new ECFactory<OneShotEC>()) );
    p.store()->addExecutionContextFactory(ecf1);

    THEN("FSM is stanby") {
        p.startAsync();
        auto infos = p.store()->getFSMInfos();
        REQUIRE(infos.listValue().size() == 1);

        auto fsm = p.store()->getFSM(std::string("FSM0.fsm"));
        REQUIRE(fsm->isNull() == false);

        auto state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "created");

        state = fsm->setFSMState("stopped");
        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "stopped");


        state = fsm->setFSMState("hoge");
        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "stopped");

        state = fsm->setFSMState("created");
        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "stopped");
    }

    THEN("FSM can bind operation from state") {
        p.startAsync();
        auto ope = p.store()->getOperation(std::string("add0.ope"));
        REQUIRE(ope->isNull() == false);

        auto fsm = p.store()->getFSM(std::string("FSM0.fsm"));
        REQUIRE(fsm->isNull() == false);

        auto state = fsm->setFSMState("stopped");
        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "stopped");

        auto bindResult = fsm->bindStateToOperation("running", ope);
        REQUIRE(bindResult.isError() == false);

        operationIsCalled = false;
        state = fsm->setFSMState("running");
        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "running");
        REQUIRE(operationIsCalled == true);
      }

    THEN("FSM can bind EC from state") {
        p.startAsync();
        auto ec = p.store()->getExecutionContext("OneShotEC0.ec");
        REQUIRE(ec->isNull() == false);

        auto fsm = p.store()->getFSM("FSM0.fsm");
        REQUIRE(fsm->isNull() == false);

        auto state = fsm->setFSMState("stopped");
        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "stopped");

        auto bindResult = fsm->bindStateToECStart("running", ec);
        REQUIRE(bindResult.isError() == false);
        bindResult = fsm->bindStateToECStop("stopped", ec);
        REQUIRE(bindResult.isError() == false);

        ec->stop();
        REQUIRE(ec->getState() == "stopped");
        state = fsm->setFSMState("running");
        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "running");
        REQUIRE(ec->getState() == "started");

        state = fsm->setFSMState("stopped");
        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "stopped");
        REQUIRE(ec->getState() == "stopped");
      }
    
    THEN("FSM can be bound from Operation") {
        p.startAsync();
        auto ope = p.store()->getOperation("add0.ope");
        REQUIRE(ope->isNull() == false);

        auto fsm = p.store()->getFSM("FSM0.fsm");
        REQUIRE(fsm->isNull() == false);

        auto state = fsm->setFSMState("stopped");
        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "stopped");

        Value conInfo{
          {"name", "con0"},
          {"type", "stateBind"},
          {"broker", "CoreBroker"},
          {"input", {
            {"info", {
              {"fullName", "FSM0.fsm"}
            }},
            {"broker", {
              {"typeName", "CoreBroker"}
            }},
            {"target", {
              {"name", "running"}
            }}
          }},
          {"output", {
            {"info", {
              {"fullName", "add0.ope"}
            }},
            {"broker", {
              {"typeName", "CoreBroker"}
            }}
          }}
        };
        ConnectionBuilder::createConnection(p.store(), conInfo);

        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "stopped");

        ope->execute();
        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "running");


      }

    THEN("FSM connection name duplication change name") {
        p.startAsync();
        auto ope = p.store()->getOperation("add0.ope");
        REQUIRE(ope->isNull() == false);
        auto ope2 = p.store()->getOperation("zero0.ope");
        REQUIRE(ope2->isNull() == false);

        auto fsm = p.store()->getFSM("FSM0.fsm");
        REQUIRE(fsm->isNull() == false);

        auto state = fsm->setFSMState("stopped");
        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "stopped");

        Value conInfo{
          {"name", "con0"},
          {"type", "stateBind"},
          {"broker", "CoreBroker"},
          {"input", {
            {"info", {
              {"fullName", "FSM0.fsm"}
            }},
            {"broker", {
              {"typeName", "CoreBroker"}
            }},
            {"target", {
              {"name", "running"}
            }}
          }},
          {"output", {
            {"info", {
              {"fullName", "add0.ope"}
            }},
            {"broker", {
              {"typeName", "CoreBroker"}
            }}
          }}
        };
        ConnectionBuilder::createConnection(p.store(), conInfo);

        Value conInfo2{
          {"name", "con0"},
          {"type", "stateBind"},
          {"broker", "CoreBroker"},
          {"input", {
            {"info", {
              {"fullName", "FSM0.fsm"}
            }},
            {"broker", {
              {"typeName", "CoreBroker"}
            }},
            {"target", {
              {"name", "running"}
            }}
          }},
          {"output", {
            {"info", {
              {"fullName", "zero0.ope"}
            }},
            {"broker", {
              {"typeName", "CoreBroker"}
            }}
          }}
        };

        auto ret = ConnectionBuilder::createConnection(p.store(), conInfo2);
        REQUIRE(ret.at("name").stringValue() != "con0");

      }

    THEN("FSM connection route duplication will fail") {
        p.startAsync();
        auto ope = p.store()->getOperation("add0.ope");
        REQUIRE(ope->isNull() == false);
        auto ope2 = p.store()->getOperation("zero0.ope");
        REQUIRE(ope2->isNull() == false);

        auto fsm = p.store()->getFSM("FSM0.fsm");
        REQUIRE(fsm->isNull() == false);

        auto state = fsm->setFSMState("stopped");
        state = fsm->getFSMState();
        REQUIRE(state.stringValue() == "stopped");

        Value conInfo{
          {"name", "con0"},
          {"type", "stateBind"},
          {"broker", "CoreBroker"},
          {"input", {
            {"info", {
              {"fullName", "FSM0.fsm"}
            }},
            {"broker", {
              {"typeName", "CoreBroker"}
            }},
            {"target", {
              {"name", "running"}
            }}
          }},
          {"output", {
            {"info", {
              {"fullName", "add0.ope"}
            }},
            {"broker", {
              {"typeName", "CoreBroker"}
            }}
          }}
        };
        ConnectionBuilder::createConnection(p.store(), conInfo);

        Value conInfo2{
          {"name", "con0"},
          {"type", "stateBind"},
          {"broker", "CoreBroker"},
          {"input", {
            {"info", {
              {"fullName", "FSM0.fsm"}
            }},
            {"broker", {
              {"typeName", "CoreBroker"}
            }},
            {"target", {
              {"name", "running"}
            }}
          }},
          {"output", {
            {"info", {
              {"fullName", "add0.ope"}
            }},
            {"broker", {
              {"typeName", "CoreBroker"}
            }}
          }}
        };

        auto ret = ConnectionBuilder::createConnection(p.store(), conInfo2);
        REQUIRE(ret.isError() == true);

      }

    THEN("Broker is running") {
      p.startAsync();
      REQUIRE(p.isRunning() == true);

      auto factory = p.store()->getBrokerFactory({{"typeName", "HTTPBroker"}, {"instanceName", "HTTPBroker0.brk"}});
      REQUIRE(factory!= nullptr);
      REQUIRE(factory->isNull() == false);
      auto proxy = factory->createProxy({{"typeName", "HTTPBroker"}, {"host", "localhost"}, {"port", 8080}});
      REQUIRE(proxy != nullptr);
      REQUIRE(proxy->isNull() == false);

      auto pInfo = proxy->getProcessInfo();

      REQUIRE(pInfo["instanceName"].stringValue() == "fsm_test");

      AND_THEN("fsm") {
        auto fsmInfos = proxy->getFSMInfos();
        REQUIRE(fsmInfos.isListValue());
        REQUIRE(fsmInfos.listValue().size() == 1);
        REQUIRE(fsmInfos[0]["instanceName"].stringValue() == "FSM0.fsm");
      }
    }
  }

  GIVEN("FSM automatic load behavior") {
    const std::string jsonStr = R"({
      "logger": { "logLevel": "OFF" },

      "operations": {
        "precreate": [
          { "typeName": "add", "instanceName": "add0.ope"},
          { "typeName": "zero", "instanceName": "zero0.ope"},
          { "typeName": "inc", "instanceName": "inc0.ope"}
        ]
      },  

      "fsms": {
          "precreate": [
              {
                  "typeName": "GenericFSM",
                  "instanceName": "FSM0.fsm",
                  "states" : [
                    {
                      "name": "created",
                      "transit": ["stopped"]
                    },
                    { 
                      "name": "stopped",
                      "transit": ["running"] 
                    },
                    { 
                      "name": "running",
                      "transit": ["stopped"],
                      "bindOperations": [
                        {
                          "instanceName": "add0.ope"
                        }
                      ]
                    }
                  ],
                  "defaultState" : "created"
              }
          ]
      },
      "ecs": {
          "precreate": [
            {
              "typeName": "OneShotEC",
              "instanceName": "OneShotEC0.ec"
            }
          ],
          "bind": {},
          "start": []
      },
      "brokers": {
          "load_paths": ["build/bin", "build/lib", "../../../build/bin"],
          "preload": ["HTTPBroker"],
          "precreate": [
            {
              "typeName": "HTTPBroker",
              "instanceName": "HTTPBroker0.brk",
              "host": "0.0.0.0",
              "port": 8080,
              "baseDir": "${ProjectDirectory}/../systemeditor"
            }
          ]
      }
    })";

    Process p("fsm_test", jsonStr);

    p.loadOperationFactory(opf1);
    p.loadOperationFactory(opf2);
    p.loadOperationFactory(opf3);

    THEN("FSM automatically bound to operation") {
        p.startAsync();
        auto ope = p.store()->getOperation(std::string("add0.ope"));
        REQUIRE(ope->isNull() == false);

        auto fsm = p.store()->getFSM(std::string("FSM0.fsm"));
        REQUIRE(fsm->isNull() == false);

        auto opes = fsm->getBoundOperations("running");
        REQUIRE (opes.size() == 1);
        REQUIRE (opes[0]->info().at("instanceName") == "add0.ope");
    }
  }
}
