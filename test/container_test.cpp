
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/process.h"


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;


SCENARIO( "Container test", "[container]" ) {
  GIVEN("Container basic behavior") {
    const std::string jsonStr = R"(
{
  "logger": { "logLevel": "DEBUG" },

  "containers": {
    "load_paths": ["test", "build/test", "../build/lib", "../build/example"],
    "preload": [
      { 
        "typeName": "MyStruct",
        "operations": [
          "addInt", "intGetter"
        ]
      }
    ],
    "precreate": [
      {
        "typeName": "MyStruct",
        "instanceName": "MyStruct0.ctn",
        "operations": [
          {
            "typeName": "addInt",
            "instanceName": "addInt0.ope"
          },
          {
            "typeName": "intGetter",
            "instanceName": "intGetter0.ope"
          }
        ]
      }
    ]
  }
}
)";

/*
                  "operations": {
                    "addInt": {"instanceName": "addInt0.ope"},
                    "intGetter": {"instanceName": "intGetter0.ope"}
                  }
                }
            }
            */
    Process p("container_test", jsonStr);
    p.startAsync();
    THEN("Container is stanby") {
      auto c = p.store()->container("MyStruct0.ctn");
      REQUIRE(!c->isNull());
      REQUIRE(c->instanceName() == "MyStruct0.ctn");

      AND_THEN("Container operation test") {
        auto cops = p.store()->operations();
        REQUIRE(cops.size() == 2);

      /*
        REQUIRE(ops.size() == 2);

        auto addInt = c->getOperation({{"instanceName", "addInt0.ope"}});
        REQUIRE(!addInt->isNull());
        addInt->call({{"data", 25}});
        auto intGetter = c->getOperation({{"instanceName", "intGetter0.ope"}});
        auto v = intGetter->invoke();
        REQUIRE(v.isIntValue());
        REQUIRE(v.intValue() == 25);
      }
      */
      }
    }
    p.stop();
  }
}
