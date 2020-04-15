
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
    THEN("Process is running") {
      p.startAsync();
      REQUIRE(p.isRunning() == true);
    }

    THEN("Process is not running") {
      REQUIRE(p.isRunning() == false);
    }
  }

  GIVEN("Process Logger behavior") {
    const int argc = 3;
    const char* argv[] = {"process_test", "-f", "nk_process_test.json"};
    Process p(argc, argv);

    THEN("Process logger") {
      REQUIRE(logger::getLogLevel() == logger::DEBUG);
    }
  }

  GIVEN("Process loads operation") {
    const int argc = 3;
    const char* argv[] = {"process_test", "-f", "nk_process_test.json"};
    Process p(argc, argv);
    THEN("Process has one operation") {
      p.startAsync();
      auto v = p.getOperationInfos();
      REQUIRE(v.listValue().size() >= 1);
      auto& op = p.getOperation({{"instanceName", "one0.ope"}});
      REQUIRE(op.isNull() == false);
    }

    THEN("Process has increment operation") {
      p.startAsync();
      auto v = p.getOperationInfos();
      REQUIRE(v.listValue().size() >= 1);
      auto&op = p.getOperation({{"instanceName", "increment0.ope"}});
      REQUIRE(op.isNull() == false);
    }
  }


  GIVEN("Process loads container") {
    const int argc = 3;
    const char* argv[] = {"process_test", "-f", "nk_process_test.json"};
    Process p(argc, argv);
    THEN("Process has MyStruct container") {
      p.startAsync();
      auto v = p.getContainerInfos();
      REQUIRE(v.listValue().size() >= 1);
      auto& ctn = p.getContainer({{"instanceName", "MyStruct0.ctn"}});
      REQUIRE(ctn.isNull() == false);
    }

  }
}
