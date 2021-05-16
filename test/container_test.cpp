
#include <iostream>
#include <juiz/logger.h>
#include <juiz/process.h>


#include "operations_for_tests.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
using namespace juiz;


SCENARIO( "Container test", "[container]" ) {
  GIVEN("Container basic behavior") {
    const std::string jsonStr = R"(
{
  "logger": { "logLevel": "WARN" },

  "containers": {
    "precreate": [
          {
            "typeName": "MyStruct",
            "instanceName": "MyStruct0.ctn",
            "operations": [
              {
                "typeName": "set",
                "instanceName": "set0.ope"
              },
              {
                "typeName": "inc",
                "instanceName": "inc0.ope"
              },
              {
                "typeName": "get",
                "instanceName": "get0.ope"
              }
            ]
          }
      ]
   }
}
)";

    auto p = juiz::process("container_test", jsonStr);
      
      p->load(cf0);
      p->load(copf0);
      p->load(copf1);
      p->load(copf2);
      
    p->startAsync();
      
    
    THEN("Container is stanby") {
      auto c = p->store()->get<ContainerAPI>("MyStruct0.ctn");
      REQUIRE(c->isNull() == false);
      REQUIRE(c->instanceName() == "MyStruct0.ctn");

      AND_THEN("Container operation test") {
        auto cops = p->store()->list<OperationAPI>();
        REQUIRE(cops.size() == 5);

        auto getter = p->store()->get<OperationAPI>("MyStruct0.ctn:get0.ope");
        REQUIRE(getter->isNull() != true);
        auto setter = p->store()->get<OperationAPI>("MyStruct0.ctn:set0.ope");
        REQUIRE(setter->isNull() != true);
        auto v = getter->call({});
        REQUIRE(Value::intValue(v) == 0);
        auto v2 = setter->call({{"value", 11}});

        auto v3 = getter->call({});
        REQUIRE(Value::intValue(v3) == 11);
      }
    }
    p->stop();
  }
}
