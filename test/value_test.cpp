
#include <iostream>
#include "juiz/logger.h"
#include "juiz/process.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
using namespace juiz;


SCENARIO( "Value test", "[value]" ) {

  GIVEN("Value test") {
    Value v{{"a0", Value{
        {"b0", Value{0}},
        {"b1", Value{1}}
        }
      }};

    THEN("Concat value access") {
      REQUIRE(v["a0.b0"].intValue() == 0);
      REQUIRE(v["a0.b1"].intValue() == 1);

    }
  }

  GIVEN("Value test") {
    Value v{{"a0", Value{
          {"b0", Value{
            {"c0", Value{0}},
            {"c1", Value{1}}
          }
        }}
      }};

    THEN("Concat value access") {
      REQUIRE(v["a0.b0.c0"].intValue() == 0);
      REQUIRE(v["a0.b0.c1"].intValue() == 1);

    }
  }
}
