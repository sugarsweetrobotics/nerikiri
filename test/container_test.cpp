
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;


SCENARIO( "Container test", "[container]" ) {
  GIVEN("Container basic behavior") {
    const int argc = 3;
    const char* argv[] = {"container_test", "-f", "nk_container_test.json"};
    Process p(argc, argv);
    THEN("Container is stanby") {
      p.startAsync();
      auto c = p.getContainer({{"instanceName", "MyStruct0.ctn"}});
      REQUIRE(!c->isNull());
      REQUIRE(c->getInstanceName() == "MyStruct0.ctn");
      AND_THEN("Container operation test") {
        auto ops = c->getOperationInfos();
        REQUIRE(ops.size() == 2);

        auto addInt = c->getOperation({{"instanceName", "addInt0.ope"}});
        REQUIRE(!addInt->isNull());
        addInt->call({{"data", 25}});
        auto intGetter = c->getOperation({{"instanceName", "intGetter0.ope"}});
        auto v = intGetter->invoke();
        REQUIRE(v.isIntValue());
        REQUIRE(v.intValue() == 25);
      }

/*
      AND_THEN("Container operation connect test") {
        auto one0 = p.getOperation({{"instanceName", "one0.ope"}});
        REQUIRE(!one0->isNull());
        auto cis = one0->getOutputConnectionInfos();
        REQUIRE(cis.listValue().size() == 1);

        auto increment0 = p.getOperation({{"instanceName", "increment0.ope"}});
        REQUIRE(!increment0->isNull());
        auto cis2 = increment0->getOutputConnectionInfos();
        REQUIRE(cis2.listValue().size() == 1);


        REQUIRE(!one0->execute().isError());
        auto intGetter = c->getOperation({{"instanceName", "intGetter0.ope"}});
        auto v = intGetter->invoke();
        REQUIRE(v.isIntValue());
        REQUIRE(v.intValue() == 2);
      }
*/
    }
  }
}
