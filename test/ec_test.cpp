
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;

SCENARIO( "ExecutionContext test", "[ec]" ) {
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

    "brokers": {
        "load_paths": ["test", "build/test", "../build/test", "../build/example"],
        "preload": ["HTTPBroker"],
        "precreate": [ 
          {
            "name": "HTTPBroker",
            "instanceName": "HTTPBroker0.brk",
            "host": "0.0.0.0",
            "port": 8080
          }
        ]
    },

    "ecs": {
        "load_paths": ["test", "build/test", "../build/test", "../build/example"],
        "preload": ["OneShotEC"],
        "precreate": [
          {
            "name": "OneShotEC",
            "instanceName": "OneShotEC0.ec"
          }
        ],
        "bind": {},
        "start": []
    }
    })";

  Process p("ec_tset", jsonStr);
  THEN("ExcutionContext is stanby") {
      p.startAsync();
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
    }
  }
}

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
