
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;

SCENARIO( "Process test", "[process]" ) {
  GIVEN("Process basic behavior") {
    Process p("process_test");
    THEN("Process is running") {
      p.startAsync();
      REQUIRE(p.isRunning() == true);
    }
    THEN("Process is not running") {
      REQUIRE(p.isRunning() == false);
    }
    THEN("Process logger") {
      REQUIRE(logger::getLogLevel() == logger::INFO);
    }
  }
}
