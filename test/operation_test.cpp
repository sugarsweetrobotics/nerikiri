
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch_all.hpp"
using namespace nerikiri;

#include "operations_for_tests.h"

SCENARIO( "Operation test", "[operaion]" ) {
  GIVEN("Operation basic behavior") {
    const std::string jsonStr = R"(
{
    "logger": { "logLevel": "ERROR" },

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
    }
}  
)";
    auto p = nerikiri::process("operation_test", jsonStr);
    p->load(opf1);
    p->load(opf2);
    p->load(opf3);
    p->startAsync();

    auto op = p->store()->get<OperationAPI>("zero0.ope");
    REQUIRE(op->isNull() == false);

    auto iop = p->store()->get<OperationAPI>("inc0.ope");
    REQUIRE(iop->isNull() == false);

    THEN("Operation is stanby") {
      auto v = op->invoke();
      REQUIRE(v.isIntValue() == true);
      REQUIRE(v.intValue() == 0);
    }

    THEN("Operation is connected and event invoke targeted operation.") {
      auto iv = iop->call({{"arg01", 0}});
      REQUIRE(iv.isIntValue() == true);
      REQUIRE(iv.intValue() == 1);
    }

    WHEN("Operation inlet is changed.") {
      auto v = iop->invoke();
      REQUIRE(v.isIntValue() == true);
      REQUIRE(v.intValue() == 2);

      auto inlets = iop->inlets();
      REQUIRE(inlets.size() == 1);
/*
      auto inlet = inlets[0];
      inlet->put(2);
      auto v2 = iop->invoke();
      REQUIRE(v2.isIntValue() == true);
      REQUIRE(v2.intValue() == 3); */
    }
  }
}
