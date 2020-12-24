
#include <iostream>
#include <memory>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"
#include "nerikiri/operation_factory.h"
//#include "nerikiri/connection_builder.h"

#include "operations_for_tests.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;




SCENARIO( "Connection test", "[ec]" ) {
  GIVEN("Connection basic behavior") {
    const std::string jsonStr = R"({
      "logger": { "logLevel": "INFO" },

      "operations": {
        "precreate": [
          {"typeName": "inc", "instanceName": "inc0.ope"},
          {"typeName": "one", "instanceName": "one0.ope"},
          {"typeName": "add", "instanceName": "add0.ope"},
          {"typeName": "zero", "instanceName": "zero0.ope"}
        ]
      }
    })";

    auto p = nerikiri::process("ec_tset", jsonStr);

    
    p->loadOperationFactory(opf1);
    p->loadOperationFactory(opf2);
    p->loadOperationFactory(opf3);
    p->loadOperationFactory(opf4);

    WHEN("Operation is stanby") {
      p->startAsync();
      auto zero0ope = p->store()->operation("zero0.ope");
      REQUIRE(zero0ope->isNull() == false);
      REQUIRE(Value::intValue(zero0ope->call({}), -1) == 0);

      auto inc0ope = p->store()->operation("inc0.ope");
      REQUIRE(inc0ope->isNull() == false);
      REQUIRE(Value::intValue(inc0ope->call({{"arg01", 3}}), -1) == 4);


      auto one0ope = p->store()->operation("one0.ope");
      REQUIRE(one0ope->isNull() == false);
      REQUIRE(Value::intValue(one0ope->call({}), -1) == 1);


      auto add0ope = p->store()->operation("add0.ope");
      REQUIRE(add0ope->isNull() == false);
      REQUIRE(Value::intValue(add0ope->call({
        {"arg01", 1}, {"arg02", 2}
      }), -1) == 3);

      Value con0Info{
        {"name", "con0"},
        {"type", "event"},
        {"broker", "CoreBroker"},
        {"inlet", {
          {"name", "arg01"}, 
          {"operation", {
            {"fullName", "inc0.ope"},
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
          }}
        }}
      };

      Value con1Info{
        {"name", "con1"},
        {"type", "event"},
        {"broker", "CoreBroker"},
        {"inlet", {
          {"name", "arg01"}, 
          {"operation", {
            {"fullName", "inc0.ope"},
            {"broker", {
              {"typeName", "CoreBroker"}
            }}
          }}
        }},
        {"outlet", {
          {"operation", {
            {"fullName", "one0.ope"},
            {"broker", {
              {"typeName", "CoreBroker"}
            }}
          }}
        }}
      };


      THEN("Operation can connected") {
        auto v = p->coreBroker()->connection()->createConnection(con0Info);
        REQUIRE(v.isError() == false);

        auto con1 = zero0ope->outlet()->connections();
        REQUIRE(con1.size() == 1);
        REQUIRE(con1[0]->fullName() == "con0");

        auto con2 = inc0ope->inlet("arg01")->connections();
        REQUIRE(con2.size() == 1);
        REQUIRE(con2[0]->fullName() == "con0");

        REQUIRE(Value::intValue(inc0ope->execute(), -1) == 2);

        zero0ope->execute();

        REQUIRE(Value::intValue(inc0ope->execute(), -1) == 1);

        AND_THEN("Exact Same Connection in inlet side fails") {
          auto v = p->coreBroker()->connection()->createConnection(con0Info);
          REQUIRE(v.isError() == true);
        }

        AND_THEN("Exact Same Connection in outlet side fails") {

          Value con0Info2 {
            {"name", "con0"},
            {"type", "event"},
            {"broker", "CoreBroker"},
            {"inlet", {
              {"name", "arg01"}, 
              {"operation", {
                {"fullName", "add0.ope"},
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
              }}
            }}
          };

          
          auto v = p->coreBroker()->connection()->createConnection(con0Info2);
          REQUIRE(v.isError() == true);


          auto con2 = add0ope->inlet("arg01")->connections();
          REQUIRE(con2.size() == 0);
          //REQUIRE(con2[0]->fullName() == "con0");

          auto con1 = zero0ope->outlet()->connections();
          REQUIRE(con1.size() == 1);
        }

        AND_THEN("Double connection") {
          auto v = p->coreBroker()->connection()->createConnection(con1Info);
          REQUIRE(v.isError() == false);
        }

        AND_THEN("Same Connection but different name fails") {
          con0Info["name"] = "con3";
          auto v = p->coreBroker()->connection()->createConnection(con0Info);
          REQUIRE(v.isError() == true);
        }

        AND_THEN("Different Connection but same name fails") {
          con1Info["name"] = "con0";
          auto v = p->coreBroker()->connection()->createConnection(con1Info);
          REQUIRE(v.isError() == true);
        }
      }
      
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


