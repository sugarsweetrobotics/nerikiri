
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"
#include "nerikiri/ec.h"
#include "nerikiri/operation_factory.h"
#include "nerikiri/process.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

using namespace nerikiri;

#include "operations_for_tests.h"


SCENARIO( "ExecutionContext test", "[ec]" ) {
  GIVEN("ExecutionContext basic behavior") {
    const std::string jsonStr = R"({
      "logger": { "logLevel": "INFO" },

      "operations": {
          "precreate": [
              {
                  "typeName": "zero",
                  "instanceName": "zero0.ope"
              }, 
              {
                  "typeName": "inc",
                  "instanceName": "inc0.ope"
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
      }
      })";

    auto p = nerikiri::process("ec_tset", jsonStr);
    
    p->loadOperationFactory(opf1);
    p->loadOperationFactory(opf2);
    p->loadOperationFactory(opf3);
    p->loadECFactory(ecf1);

    THEN("ExcutionContext is stanby") {
      p->startAsync();
        
        auto ecc = p->store()->container("OneShotEC0.ec");
        REQUIRE(ecc->isNull() == false);
        
        auto getter = p->store()->operation("OneShotEC0.ec:get_state.ope");
        REQUIRE(getter->isNull() == false);
        
        auto startOp = p->store()->operation("OneShotEC0.ec:activate_state_started.ope");
        REQUIRE(startOp->isNull() == false);
        auto stopOp = p->store()->operation("OneShotEC0.ec:activate_state_stopped.ope");
        REQUIRE(stopOp->isNull() == false);
        
        auto op = p->store()->operation("zero0.ope");
        REQUIRE(op->isNull() == false);
        
        AND_THEN("Bind and start EC") {
            
            Value conInfo{
              {"name", "con0"},
              {"type", "EVENT"},
              {"broker", "CoreBroker"},
              {"outlet", {
                {"operation", {
                  {"fullName", "OneShotEC0.ec:activate_state_started.ope"},
                  {"broker", {
                    {"typeName", "CoreBroker"}
                  }}
                }}
              }},
              {"inlet", {
                {"name", "__event__"},
                {"operation", {
                  {"fullName", "zero0.ope"},
                  {"broker", {
                    {"typeName", "CoreBroker"}
                  }}
                }
                }
              }}
            };
            auto v = p->coreBroker()->connection()->createConnection(conInfo);

            REQUIRE(startOp->outlet()->connections().size() == 1);
            
            
            REQUIRE(getter->call({}).stringValue() == "stopped");
            
            
            operationIsCalled = false;
            REQUIRE(operationIsCalled == false);
            
            startOp->execute();
            
            REQUIRE(operationIsCalled == true);
            
            REQUIRE(getter->call({}).stringValue() == "started");
        }
      //auto ecInfos = p->store()->executionContexts();
      //REQUIRE(ecInfos.size() == 1);
    }

    THEN("ExecutionContext can bind to operation") {
      p->startAsync();
        /*
      auto ec = p->store()->executionContext("OneShotEC0.ec");
      REQUIRE(ec->isNull() == false);

      auto op = p->store()->operation("zero0.ope");
      //auto broker = p.store()->brokerFactory("CoreBroker")->createProxy({{"fullName", "coreBrokerProxy"}});
      //auto res = ec->bind("zero0.ope", broker);
      auto result = ec->bind(op);
      REQUIRE(result.isError() == false);

      operationIsCalled = false;
      REQUIRE(operationIsCalled == false);
      ec->start();
      REQUIRE(operationIsCalled == true);
         */

    }
  }
}
      /*
      auto ec = p.getExecutionContext({{"instanceName", "OneShotEC0.ec"}});
      REQUIRE(!ec->isNull());
      auto brk = p.createBrokerProxy({{"name", "HTTPBroker"}, {"host", "localhost"}, {"port", 8080}});
      REQUIRE(brk->isNull() == false);
      auto ops = ec->getBoundOperationInfos();
      REQUIRE(ops.listValue().size() == 0);

      ec->bind({{"instanceName", "one0.ope"}}, brk);

      auto ops2 = ec->getBoundOperationInfos();
      REQUIRE(ops2.listValue().size() == 1);

      AND_THEN("ExecutionContext ec test") {
        ec->unbind(ops2[0]);
        auto ops2 = ec->getBoundOperationInfos();
        REQUIRE(ops2.listValue().size() == 0);
      }
    }*/
    
  

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
}
*/
