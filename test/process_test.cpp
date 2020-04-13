
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;


SCENARIO( "Process test", "[process]" ) {
  GIVEN("Process basic behavior") {
    const int argc = 3;
    const char* argv[] = {"process_test", "-f", "nk_process_test.json"};
    Process p(argc, argv);

  logger::setLogLevel(logger::OFF);
    THEN("Process is running") {
      p.startAsync();
      REQUIRE(p.isRunning() == true);
    }

    THEN("Process is not running") {
      REQUIRE(p.isRunning() == false);
    }

  }
  
}
