
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;

SCENARIO( "ExecutionContext test", "[ec]" ) {
  const int argc = 3;
  const char* argv[] = {"ec_test", "-f", "nk_ec_test.json"};
  Process p(argc, argv);
  GIVEN("ExecutionContext basic behavior") {
    THEN("ExcutionContext is stanby") {
      p.startAsync();
      auto ec = p.getExecutionContext({{"instanceName", "TimerEC0.ec"}});
      REQUIRE(!ec->isNull());
      auto ops = ec->getBoundOperationInfos();
      REQUIRE(ops.listValue().size() == 1);
      AND_THEN("ExecutionContext ec test") {
      }
    }
  }
}
