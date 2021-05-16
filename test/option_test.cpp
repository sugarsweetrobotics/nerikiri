
#include <iostream>
#include "../src/nerikiri/argparse.h"


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
using namespace juiz;


SCENARIO( "Arg Parser", "[argparse]" ) {
  GIVEN("Arg Parse default behavior") {
    ArgParser parser;
    parser.option("-a", "--aoption", "Default Option", false);
    parser.option("-b", "--boption", "B type option", false, std::string("stringArgument"));
    THEN("default info") {
        std::vector<std::string> args{"parse_test"};
        auto options = parser.parse(std::move(args));
        REQUIRE(options.get<bool>("aoption") == false);
    }

    THEN("non default") {
        std::vector<std::string> args{"parse_test", "-a"};
        auto options = parser.parse(std::move(args));
        REQUIRE(options.get<bool>("aoption") == true);
    }

    THEN("default str") {
        std::vector<std::string> args{"parse_test"};
        auto options = parser.parse(std::move(args));
        REQUIRE(options.get<std::string>("boption") == "stringArgument");
    }

    THEN("non default str") {
        std::vector<std::string> args{"parse_test", "-b", "newArgument"};
        auto options = parser.parse(std::move(args));
        REQUIRE(options.get<std::string>("boption") == "newArgument");
    }
  }
  
}
