
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
    }
  })";

  Process p("ec_tset", jsonStr);

  p.loadOperationFactory(opf1);
  p.loadOperationFactory(opf2);
  p.loadOperationFactory(opf3);

  THEN("FSM is stanby") {
      p.startAsync();
      auto infos = p.store()->getFSMInfos();
      REQUIRE(infos.listValue().size() == 1);

      auto fsm = p.store()->getFSM("FSM0.fsm");
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

  THEN("FSM can bind operation to state") {
      p.startAsync();
      auto ope = p.store()->getOperation("add0.ope");
      REQUIRE(ope->isNull() == false);

      auto fsm = p.store()->getFSM("FSM0.fsm");
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
  }
}

/*
SCENARIO( "ExecutionContext remote test", "[ec]" ) {
  GIVEN("ExecutionContext basic behavior") {
    const std::string jsonStr = R"({
    "logger": { "logLevel": "OFF" },

    "operations": {
        "load_paths": ["test", "build/test", "../build/test", "../build/example"],
        "preload": ["one", "increment"],
        "precreate": [
            {
                "name": "one",
                "instanceName": "one0.ope"
            }, 
            {
                "name": "increment",
                "instanceName": "increment0.ope"
            }
        ]
    },

    "ecs": {
        "load_paths": ["test", "build/test", "../build/test", "../build/example"],
        "preload": ["TimerEC"],
        "precreate": [
          {
            "name": "TimerEC",
            "instanceName": "TimerEC0.ec",
            "rate": 1.0
          }
        ],
        "bind": {},
        "start": []
    }
    })";

    Process p("ec_tset", jsonStr);

    THEN("ExcutionContext can bind operation") {
      p.startAsync();
      auto ec = p.getExecutionContext({{"instanceName", "TimerEC0.ec"}});
      REQUIRE(!ec->isNull());
      auto ops = ec->getBoundOperationInfos();
      REQUIRE(ops.listValue().size() == 0);
      auto op = p.getOperation({{"instanceName", "one0.ope"}});
      ec->bind(op);
      auto ops2 = ec->getBoundOperationInfos();
      REQUIRE(ops2.listValue().size() == 1);
      AND_THEN("ExecutionContext bind operation") {
        ec->unbind(op->info());
        auto ops3 = ec->getBoundOperationInfos();
        REQUIRE(ops3.listValue().size() == 0);
      }
    }
  }
  */


