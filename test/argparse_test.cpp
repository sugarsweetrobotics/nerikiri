
#include <iostream>
#include <memory>
#include <juiz/utils/argparse.h>


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
using namespace juiz;

TEST_CASE( "Simple one input test", "[args]" ) {
    ArgParser parser;
    parser.option<std::string>("-f", "--filename", "This is help for filename", true, "file.in");

    const char* args[] = {"program_name", "-f", "my_file.in"};

    auto options = parser.parse(3, args);
    REQUIRE(options.program_name == "program_name");
    REQUIRE(options.results.size() == 1);
    REQUIRE(getValue<std::string>(options.results["filename"], "default_filename.in") == "my_file.in");
    REQUIRE(options.unknown_args.size() == 0);
}

TEST_CASE( "Simple one input but default test", "[args]" ) {
    ArgParser parser;
    parser.option<std::string>("-f", "--filename", "This is help for filename", true, "file.in");

    const char* args[] = {"program_name"};

    auto options = parser.parse(1, args);
    REQUIRE(options.program_name == "program_name");
    REQUIRE(options.results.size() == 1);
    REQUIRE(getValue<std::string>(options.results["filename"], "default_filename.in") == "file.in");
    REQUIRE(options.unknown_args.size() == 0);
}

TEST_CASE( "Default test", "[args]" ) {
    const char* args[] = {"program_name"};
    ArgParser parser;
    auto options = parser.parse(1, args);
    REQUIRE(options.program_name == "program_name");
    REQUIRE(options.results.size() == 0);
    REQUIRE(options.unknown_args.size() == 0);
}
