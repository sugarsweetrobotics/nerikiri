
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch_all.hpp"

#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"


using namespace nerikiri;

SCENARIO( "Process test", "[process]" ) {
  GIVEN("Process basic behavior") {
    auto p = nerikiri::process("process_test");
    /*
    THEN("Process is running") {
      p->startAsync();
      REQUIRE(p->isRunning() == true);
    }
    THEN("Process is not running") {
      REQUIRE(p->isRunning() == false);
    }
    */
    //THEN("Process logger") {
    //  ;
      //REQUIRE(logger::getLogLevel() == logger::INFO);
    //}
  }
}
