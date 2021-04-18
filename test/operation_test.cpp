
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch_all.hpp"
using namespace nerikiri;


SCENARIO( "Operation test", "[operaion]" ) {
  GIVEN("Operation basic behavior") {
    const std::string jsonStr = R"(
{
    "logger": { "logLevel": "ERROR" },

    "operations": {
        "load_paths": ["test", "build/test", "../build/lib", "../build/example/simple_example"],
        "preload": ["one", "increment"],
        "precreate": [
            {
                "typeName": "one",
                "instanceName": "one0.ope"
            }, 
            {
                "typeName": "increment",
                "instanceName": "increment0.ope"
            }
        ]
    }
}  
)";
    auto p = nerikiri::process("operation_test", jsonStr);
    p->startAsync();

    auto op = p->store()->operation("one0.ope");
    REQUIRE(op->isNull() == false);

    auto iop = p->store()->operation("increment0.ope");
    REQUIRE(iop->isNull() == false);

    THEN("Operation is stanby") {
      auto v = op->invoke();
      REQUIRE(v.isIntValue() == true);
      REQUIRE(v.intValue() == 1);
    }

    THEN("Operation is connected and event invoke targeted operation.") {
      auto iv = iop->call({{"data", 1}});
      REQUIRE(iv.isIntValue() == true);
      REQUIRE(iv.intValue() == 2);
    }

    WHEN("Operation inlet is changed.") {
      auto v = iop->invoke();
      REQUIRE(v.isIntValue() == true);
      REQUIRE(v.intValue() == 1);

      auto inlets = iop->inlets();
      REQUIRE(inlets.size() == 1);

      auto inlet = inlets[0];
      inlet->put(2);
      auto v2 = iop->invoke();
      REQUIRE(v2.isIntValue() == true);
      REQUIRE(v2.intValue() == 3);
    }
  }
}
