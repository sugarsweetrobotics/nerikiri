
#include <iostream>
#include <memory>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"
#include "nerikiri/operation_factory.h"
// #include "nerikiri/connection_builder.h"

#include "operations_for_tests.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;


SCENARIO( "FSM test", "[ec]" ) {
  GIVEN("FSM basic behavior") {
    const std::string jsonStr = R"({
      "logger": { "logLevel": "WARN" },

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

    auto p = nerikiri::process("fsm_test", jsonStr);

    p->loadOperationFactory(opf1);
    p->loadOperationFactory(opf2);
    p->loadOperationFactory(opf3);
    p->loadECFactory(ecf1);


    THEN("FSM is stanby") {
        p->startAsync();
        auto fsms = p->store()->fsms();
        REQUIRE(fsms.size() == 1);

        auto fsm = p->store()->fsm(std::string("FSM0.fsm"));
        REQUIRE(fsm->isNull() == false);

        auto state = fsm->currentFsmState();
        REQUIRE(state->fullName() == "created");

        auto v = fsm->setFSMState("stopped");
        state = fsm->currentFsmState();
        REQUIRE(state->fullName() == "stopped");


        v = fsm->setFSMState("hoge");
        state = fsm->currentFsmState();
        REQUIRE(state->fullName() == "stopped");

        v = fsm->setFSMState("created");
        state = fsm->currentFsmState();
        REQUIRE(state->fullName() == "stopped");
    }

    THEN("FSM can bind operation from state") {
        p->startAsync();
        auto ope = p->store()->operation(std::string("add0.ope"));
        REQUIRE(ope->isNull() == false);

        auto fsm = p->store()->fsm(std::string("FSM0.fsm"));
        REQUIRE(fsm->isNull() == false);

        auto v = fsm->setFSMState("stopped");
        auto state = fsm->currentFsmState();
        REQUIRE(state->fullName() == "stopped");

        auto bindResult = fsm->fsmState("running")->bind(ope);
        REQUIRE(bindResult.isError() == false);

        operationIsCalled = false;
        v = fsm->setFSMState("running");
        state = fsm->currentFsmState();
        REQUIRE(state->fullName() == "running");
        REQUIRE(operationIsCalled == true);
      }

    THEN("FSM can bind EC from state") {
        p->startAsync();
        auto ec = p->store()->executionContext("OneShotEC0.ec");
        REQUIRE(ec->isNull() == false);

        auto fsm = p->store()->fsm("FSM0.fsm");
        REQUIRE(fsm->isNull() == false);

        auto v = fsm->setFSMState("stopped");
        auto state = fsm->currentFsmState();
        REQUIRE(state->fullName() == "stopped");

        auto bindResult = fsm->fsmState("running")->bind(ec->startedState());
        REQUIRE(bindResult.isError() == false);
        bindResult = fsm->fsmState("stopped")->bind(ec->stoppedState());
        REQUIRE(bindResult.isError() == false);

        ec->stop();
        REQUIRE(ec->isStopped());
        auto v2 = fsm->setFSMState("running");
        state = fsm->currentFsmState();
        REQUIRE(state->fullName()  == "running");
        REQUIRE(ec->isStarted());

        auto v3 = fsm->setFSMState("stopped");
        state = fsm->currentFsmState();
        REQUIRE(state->fullName() == "stopped");
        REQUIRE(ec->isStopped());
      }
    
    THEN("FSM can be bound from Operation") {
        p->startAsync();
        auto ope = p->store()->operation("add0.ope");
        REQUIRE(ope->isNull() == false);

        auto fsm = p->store()->fsm("FSM0.fsm");
        REQUIRE(fsm->isNull() == false);

        auto v1 = fsm->setFSMState("stopped");
        auto s1 = fsm->currentFsmState();
        REQUIRE(s1->fullName() == "stopped");

        Value conInfo{
          {"name", "con0"},
          {"type", "EVENT"},
          {"broker", "CoreBroker"},
          {"inlet", {
            {"name", "running"},
            {"fsm", {
              {"fullName", "FSM0.fsm"},
              {"broker", {
                {"typeName", "CoreBroker"}
              }}
            }}
          }},
          {"outlet", {
            {"operation", {
              {"fullName", "add0.ope"},
              {"broker", {
                {"typeName", "CoreBroker"}
              }}
            }},
            
          }}
        };

        auto v = p->coreBroker()->connection()->createConnection(conInfo);
        //p->coreBroker()->createConnection(p->store(), conInfo);

        auto state2 = fsm->currentFsmState();
        REQUIRE(state2->fullName() == "stopped");

        ope->execute();
        auto state3 = fsm->currentFsmState();
        REQUIRE(state3->fullName() == "running");


      }

    THEN("FSM connection name duplication change name") {
        p->startAsync();
        auto ope = p->store()->operation("add0.ope");
        REQUIRE(ope->isNull() == false);
        auto ope2 = p->store()->operation("zero0.ope");
        REQUIRE(ope2->isNull() == false);

        auto fsm = p->store()->fsm("FSM0.fsm");
        REQUIRE(fsm->isNull() == false);

        auto v = fsm->setFSMState("stopped");
        auto state2 = fsm->currentFsmState();
        REQUIRE(state2->fullName() == "stopped");

        Value conInfo{
          {"name", "con0"},
          {"type", "EVENT"},
          {"broker", "CoreBroker"},
          {"inlet", {
            {"name", "running"},
            {"fsm", {
              {"fullName", "FSM0.fsm"},
              {"broker", {
                {"typeName", "CoreBroker"}
              }}
            }}
          }},
          {"outlet", {
            {"operation", {
              {"fullName", "add0.ope"},
              {"broker", {
                {"typeName", "CoreBroker"}
              }}
            }},
            
          }}
        };

        auto v3 = p->coreBroker()->connection()->createConnection(conInfo);
//        ConnectionBuilder::createConnection(p->store(), conInfo);

        Value conInfo2 {
          {"name", "con0"},
          {"type", "EVENT"},
          {"broker", "CoreBroker"},
          {"inlet", {
            {"name", "running"},
            {"fsm", {
              {"fullName", "FSM0.fsm"},
              {"broker", {
                {"typeName", "CoreBroker"}
              }}
            }}
          }},
          {"outlet", {
            {"operation", {
              {"fullName", "zero0.ope"},
              {"broker", {
                {"typeName", "CoreBroker"}
              }}
            }},
            
          }}
        };

        auto ret = p->coreBroker()->connection()->createConnection(conInfo);
        // auto ret = ConnectionBuilder::createConnection(p->store(), conInfo2);
        REQUIRE(ret.at("fullName").stringValue() != "con0");

      }

    THEN("FSM connection route duplication will fail") {
        p->startAsync();
        auto ope = p->store()->operation("add0.ope");
        REQUIRE(ope->isNull() == false);
        auto ope2 = p->store()->operation("zero0.ope");
        REQUIRE(ope2->isNull() == false);

        auto fsm = p->store()->fsm("FSM0.fsm");
        REQUIRE(fsm->isNull() == false);

        auto v1 = fsm->setFSMState("stopped");
        auto state1 = fsm->currentFsmState();
        REQUIRE(state1->fullName() == "stopped");

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

        auto v = p->coreBroker()->connection()->createConnection(conInfo);
        // ConnectionBuilder::createConnection(p->store(), conInfo);

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

        auto ret = p->coreBroker()->connection()->createConnection(conInfo);
        //auto ret = ConnectionBuilder::createConnection(p->store(), conInfo2);
        REQUIRE(ret.isError() == true);

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
                      "transit": ["stopped"]
                    }
                  ],
                  "defaultState" : "created"
              }
          ],
          "bind": {
            "operations": [
              {
                "fsm": {
                  "fullName": "FSM0.fsm",
                  "state": "running"
                },
                "operation": {
                  "fullName": "add0.ope",
                  "argument": {"data": 0}
                }
              }
            ]

          }
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

    auto p = nerikiri::process("fsm_test", jsonStr);

    p->loadOperationFactory(opf1);
    p->loadOperationFactory(opf2);
    p->loadOperationFactory(opf3);

    THEN("FSM automatically bound to operation") {
        p->startAsync();
        auto ope = p->store()->operation(std::string("add0.ope"));
        REQUIRE(ope->isNull() == false);

        auto fsm = p->store()->fsm(std::string("FSM0.fsm"));
        REQUIRE(fsm->isNull() == false);

        auto opes = fsm->fsmState("running")->boundOperations();
        REQUIRE (opes.size() == 1);
        REQUIRE (opes[0]->info().at("instanceName") == "add0.ope");
    }
  }
}
