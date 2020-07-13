
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


SCENARIO( "ExecutionContext test", "[ec]" ) {
  GIVEN("ExecutionContext basic behavior") {
  const std::string jsonStr = R"({
    "logger": { "logLevel": "OFF" },

    "operations": {
      "precreate": [
        {"typeName": "inc", "instanceName": "inc0.ope"},
        {"typeName": "zero", "instanceName": "zero0.ope"}
      ]
    }
  })";

  Process p("ec_tset", jsonStr);

  auto opf1 = std::shared_ptr<OperationFactory>( new OperationFactory{
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

  auto opf2 = std::shared_ptr<OperationFactory>( new OperationFactory{
    { {"typeName", "zero"},
      {"defaultArg", { }}
    },
    [](const Value& arg) -> Value {
      operationIsCalled = true;
      return Value(0);
    }
  });


  p.loadOperationFactory(opf1);
  p.loadOperationFactory(opf2);

  THEN("Operation is stanby") {
      p.startAsync();
      auto ope1 = p.store()->getOperation("zero0.ope");
      REQUIRE(ope1->isNull() == false);

      auto ope2 = p.store()->getOperation("inc0.ope");
      REQUIRE(ope2->isNull() == false);

  }

  THEN("Operation can connected") {

      p.startAsync();
      auto ope1 = p.store()->getOperation("zero0.ope");
      REQUIRE(ope1->isNull() == false);

      auto ope2 = p.store()->getOperation("inc0.ope");
      REQUIRE(ope2->isNull() == false);

    
      Value conInfo{
        {"name", "con0"},
        {"type", "stateBind"},
        {"broker", "CoreBroker"},
        {"input", {
          {"info", {
            {"fullName", "inc0.ope"}
          }},
          {"broker", {
            {"typeName", "CoreBroker"}
          }}
          
        }},
          {"target", {
            {"name", "arg01"}
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
      ConnectionBuilder::createConnection(p.store(), conInfo);
      
      auto conInfo1 = ope1->getConnectionInfos();
      REQUIRE(conInfo1.at("output").listValue().size() == 1);

      auto conInfo2 = ope2->getConnectionInfos();
      REQUIRE(conInfo2.at("input").at("arg01").listValue().size() == 1);
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


