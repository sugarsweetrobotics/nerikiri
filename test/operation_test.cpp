
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;


SCENARIO( "Operation test", "[operaion]" ) {
  GIVEN("Operation basic behavior") {
    const int argc = 3;
    const char* argv[] = {"operaion_test", "-f", "nk_operation_test.json"};
    Process p(argc, argv);
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
