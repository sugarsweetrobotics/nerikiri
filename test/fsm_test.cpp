
#include <iostream>
#include <memory>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"
#include "nerikiri/operation_factory.h"
// #include "nerikiri/connection_builder.h"

#include "operations_for_tests.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
using namespace nerikiri;


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

      "containers": {
        "precreate": [
          {
            "typeName": "MyStruct",
            "instanceName": "MyStruct0.ctn",
            "operations": [
              {
                "typeName": "set",
                "instanceName": "set0.ope"
              },
              {
                "typeName": "inc",
                "instanceName": "inc0.ope"
              },
              {
                "typeName": "get",
                "instanceName": "get0.ope"
              }
            ]
          }
        ]
      },

      "fsms": {
          "precreate": [
              {
                  "typeName": "GenericFSM",
                  "fullName": "FSM0.fsm",
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
              "fullName": "OneShotEC0.ec"
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
      
    p->loadOperationFactory(opf3);
    p->loadContainerFactory(cf0);
    p->loadContainerOperationFactory(copf1);
    p->loadContainerOperationFactory(copf2);
    
      
    p->startAsync();
    REQUIRE(p->store()->list<ContainerAPI>().size() == 3); // FSM and EC

    auto fsm = p->store()->get<ContainerAPI>("FSM0.fsm");
    REQUIRE(fsm->isNull() == false);
      
    auto ec = p->store()->get<ContainerAPI>("OneShotEC0.ec");
    REQUIRE(ec->isNull() == false);
    
    auto ec_getter = p->store()->get<OperationAPI>("OneShotEC0.ec:get_state.ope");
    REQUIRE(ec_getter->isNull() == false);

    auto getter = p->store()->get<OperationAPI>("FSM0.fsm:get_state.ope");
    REQUIRE(getter->isNull() == false);

    auto currentState0 = getter->call({});
    REQUIRE(currentState0.stringValue() == "created");

    auto act_stopped = p->store()->get<OperationAPI>("FSM0.fsm:activate_state_stopped.ope");
    REQUIRE(act_stopped->isNull() == false);
      
    REQUIRE(p->store()->get<OperationAPI>("FSM0.fsm:activate_state_hoge.ope")->isNull() == true);
      
    auto act_run = p->store()->get<OperationAPI>("FSM0.fsm:activate_state_running.ope");
    REQUIRE(act_run->isNull() == false);
      
    auto result = act_stopped->execute();
    currentState0 = getter->call({});
    REQUIRE(currentState0.stringValue() == "stopped");

      
    THEN("Activate Stopped and Running") {

        //auto result2 = act_hoge->execute();

        auto curState3 = getter->call({});
        REQUIRE(curState3.stringValue() == "stopped");


        auto result3 = act_run->execute();

        auto curState4 = getter->call({});
        REQUIRE(curState4.stringValue() == "running");

    }
      
    auto add_ope = p->store()->get<OperationAPI>(std::string("add0.ope"));
    REQUIRE(add_ope->isNull() == false);

    THEN("FSM can bind to operation from state") {
        Value conInfo{
          {"name", "con0"},
          {"type", "EVENT"},
          {"broker", "CoreBroker"},
          {"inlet", {
            {"name", "__event__"},
            {"operation", {
              {"fullName", "FSM0.fsm:activate_state_stopped.ope"},
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
            }
            }
          }}
        };
        auto v = p->coreBroker()->connection()->createConnection(conInfo);
        REQUIRE(add_ope->outlet()->connections().size() == 1);

        AND_THEN("Run Operation and FSM State Change") {
            add_ope->execute();

            auto curState2 = getter->call({});
            REQUIRE(curState2.stringValue() == "stopped");
        } 
    }


    THEN("FSM can bind from state to operation") {
        auto container = p->store()->get<ContainerAPI>("MyStruct0.ctn");
        REQUIRE(container->isNull() == false);

        auto inc = p->store()->get<OperationAPI>(std::string("MyStruct0.ctn:inc0.ope"));
        REQUIRE(inc->isNull() == false);

        auto get = p->store()->get<OperationAPI>(std::string("MyStruct0.ctn:get0.ope"));
        REQUIRE(get->isNull() == false);

        Value conInfo{
          {"name", "con0"},
          {"type", "EVENT"},
          {"broker", "CoreBroker"},
          {"outlet", {
            {"operation", {
              {"fullName", "FSM0.fsm:activate_state_running.ope"},
              {"broker", {
                {"typeName", "CoreBroker"}
              }}
            }}
          }},
          {"inlet", {
            {"name", "__event__"},
            {"operation", {
              {"fullName", "MyStruct0.ctn:inc0.ope"},
              {"broker", {
                {"typeName", "CoreBroker"}
              }}
            }
            }
          }}
        };
        auto v = p->coreBroker()->connection()->createConnection(conInfo);
        REQUIRE(act_run->outlet()->connections().size() == 1);

        AND_THEN("Activate State and Operation is called") {
            auto curValue1 = get->call({});
            REQUIRE(curValue1.intValue() == 0);

            act_run->execute();

            auto curState2 = getter->call({});
            REQUIRE(curState2.stringValue() == "running");

            auto curValue2 = get->call({});
            REQUIRE(curValue2.intValue() == 1);
        }
    }
  

    THEN("FSM can bind EC from state") {
        auto ecc = p->store()->get<ContainerAPI>("OneShotEC0.ec");
        REQUIRE(ecc->isNull() == false);
        
        Value conInfo{
          {"name", "con0"},
          {"type", "EVENT"},
          {"broker", "CoreBroker"},
          {"outlet", {
            {"operation", {
              {"fullName", "FSM0.fsm:activate_state_running.ope"},
              {"broker", {
                {"typeName", "CoreBroker"}
              }}
            }}
          }},
          {"inlet", {
            {"name", "__event__"},
            {"operation", {
              {"fullName", "OneShotEC0.ec:activate_state_started.ope"},
              {"broker", {
                {"typeName", "CoreBroker"}
              }}
            }
            }
          }}
        };
        
        auto v = p->coreBroker()->connection()->createConnection(conInfo);
        REQUIRE(act_run->outlet()->connections().size() == 1);

        AND_THEN("FSM State change and EC state change.") {
            REQUIRE(getter->call({}).stringValue() == "stopped");
            REQUIRE(ec_getter->call({}).stringValue() == "stopped");
            act_run->execute();
        
            REQUIRE(getter->call({}).stringValue() == "running");
            REQUIRE(ec_getter->call({}).stringValue() == "started");
            
        }
    }
  }
    /*
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
              }}
          }}
        };

        auto v = p->coreBroker()->connection()->createConnection(conInfo);
        // ConnectionBuilder::createConnection(p->store(), conInfo);

        Value conInfo2{
          {"name", "con0"},
          {"type", "stateBind"},
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
  */
}
