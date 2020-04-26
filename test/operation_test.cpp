
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;


SCENARIO( "Operation test", "[operaion]" ) {
  GIVEN("Operation basic behavior") {
    const std::string jsonStr = R"(
{
    "logger": { "logLevel": "ERROR" },

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
    }
}  
)";
    Process p("operation_test", jsonStr);
    THEN("Operation is stanby") {
      p.startAsync();
      auto op = p.getOperation({{"instanceName", "one0.ope"}});
      auto v = op->invoke();
      REQUIRE(v.isIntValue() == true);
      REQUIRE(v.intValue() == 1);
    }

    THEN("Operation is connected and event invoke targeted operation.") {
      p.startAsync();
      auto iop = p.getOperation({{"instanceName", "increment0.ope"}});
      auto iv = iop->call({{"data", 1}});
      REQUIRE(iv.isIntValue() == true);
      REQUIRE(iv.intValue() == 2);
    }
  }
}
