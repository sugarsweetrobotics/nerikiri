
#include <iostream>
#include "juiz/logger.h"
#include "juiz/value.h"
#include "juiz/utils/yaml.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
using namespace juiz;

TEST_CASE("Map value access", "") {
    SECTION("map value of string access", "") {
        REQUIRE(yaml::toValue("{hoge: foo, hello: world}").isObjectValue() == true);
        auto lst = yaml::toValue("{hoge: foo, hello: world}").objectValue();
        REQUIRE(lst["hoge"].stringValue() == "foo");
        REQUIRE(lst["hello"].stringValue() == "world");
    }

    SECTION("map value of list float access", "") {
        REQUIRE(yaml::toValue("{hoge: [1.0, 2.0, 3.0]}").isObjectValue() == true);
        auto obj = yaml::toValue("{hoge: [1.0, 2.0, 3.0]}").objectValue();
        auto lst = obj["hoge"].listValue();
        REQUIRE(lst[0].doubleValue() == 1.0);
        REQUIRE(lst[1].doubleValue() == 2.0);
        REQUIRE(lst[2].doubleValue() == 3.0);
    }

    SECTION("String is not list") {
        REQUIRE(yaml::toValue("hoge").isListValue() == false);
        REQUIRE(yaml::toValue("1054").isListValue() == false);
        REQUIRE(yaml::toValue("12act").isListValue() == false);
        REQUIRE(yaml::toValue("FALSE").isListValue() == false);
    }
}

TEST_CASE("Sequence value access", "") {
    SECTION("sequence value of int access", "") {
        REQUIRE(yaml::toValue("[1, 2, 3]").isListValue() == true);
        auto lst = yaml::toValue("[1, 2, 3]").listValue();
        REQUIRE(lst[0].intValue() == 1);
        REQUIRE(lst[1].intValue() == 2);
        REQUIRE(lst[2].intValue() == 3);
    }

    SECTION("String is not list") {
        REQUIRE(yaml::toValue("hoge").isListValue() == false);
        REQUIRE(yaml::toValue("1054").isListValue() == false);
        REQUIRE(yaml::toValue("12act").isListValue() == false);
        REQUIRE(yaml::toValue("FALSE").isListValue() == false);
    }
}


TEST_CASE("String value access", "") {
    SECTION("string value base 10 access", "") {
        REQUIRE(yaml::toValue("hello,world").isStringValue() == true);
        REQUIRE(yaml::toValue("hello,world").stringValue() ==  std::string("hello,world"));

        REQUIRE(yaml::toValue("hello, world").isStringValue() == true);
        REQUIRE(yaml::toValue("hello, world").stringValue() ==  std::string("hello, world"));

        REQUIRE(yaml::toValue("0.0.0.0").isStringValue() == true);
        REQUIRE(yaml::toValue("0.0.0.0").stringValue() ==  std::string("0.0.0.0"));
    }

    SECTION("Value is not string") {
        REQUIRE(yaml::toValue("1.3").isStringValue() == false);
        REQUIRE(yaml::toValue("1054").isStringValue() == false);
        REQUIRE(yaml::toValue("{hoge: foo}").isStringValue() == false);
        REQUIRE(yaml::toValue("FALSE").isStringValue() == false);
    }
}


TEST_CASE("Float value access", "") {
    SECTION("float value base 10 access", "") {
        REQUIRE(yaml::toValue("1.0").isDoubleValue() == true);
        REQUIRE(yaml::toValue("1.0").doubleValue() ==  1.0);
    }

    SECTION("String is not float") {
        REQUIRE(yaml::toValue("hoge").isDoubleValue() == false);
        REQUIRE(yaml::toValue("1054").isDoubleValue() == false);
        REQUIRE(yaml::toValue("12act").isDoubleValue() == false);
        REQUIRE(yaml::toValue("FALSE").isDoubleValue() == false);
    }
}

TEST_CASE("Int value access", "") {
    SECTION("int value base 10 access", "") {
        REQUIRE(yaml::toValue("10").isIntValue() == true);
        REQUIRE(yaml::toValue("10").intValue() ==  10);
        REQUIRE(yaml::toValue("10_000").isIntValue() == true);
        REQUIRE(yaml::toValue("10_000").intValue() ==  10000);
        REQUIRE(yaml::toValue("2500").isIntValue() == true);
        REQUIRE(yaml::toValue("2500").intValue() ==  2500);
        REQUIRE(yaml::toValue("-10").isIntValue() == true);
        REQUIRE(yaml::toValue("-10").intValue() ==  -10);
        REQUIRE(yaml::toValue("+10").isIntValue() == true);
        REQUIRE(yaml::toValue("+10").intValue() ==  10);
        REQUIRE(yaml::toValue("0").isIntValue() == true);
        REQUIRE(yaml::toValue("0").intValue() ==  0);
        REQUIRE(yaml::toValue("+0").isIntValue() == true);
        REQUIRE(yaml::toValue("+0").intValue() ==  0);
    }

    SECTION("Int value base 2 access") {
        REQUIRE(yaml::toValue("0b0000").isIntValue() == true);
        REQUIRE(yaml::toValue("0b0000").intValue() ==  0);
        REQUIRE(yaml::toValue("0b0110").isIntValue() == true);
        REQUIRE(yaml::toValue("0b0110").intValue() ==  6);
        REQUIRE(yaml::toValue("0b111").isIntValue() == true);
        REQUIRE(yaml::toValue("0b111").intValue() == 7);

        REQUIRE(yaml::toValue("0b0000_0010").isIntValue() == true);
        REQUIRE(yaml::toValue("0b0000_0010").intValue() == 2);
    }

    SECTION("Concat int value base 8 access") {
        REQUIRE(yaml::toValue("07").isIntValue() ==  true);
        REQUIRE(yaml::toValue("07").intValue() == 7);
        REQUIRE(yaml::toValue("010").isIntValue() == true);
        REQUIRE(yaml::toValue("010").intValue() ==  8);
    }

    SECTION("Concat int value base 16 access") {
        REQUIRE(yaml::toValue("0x10").isIntValue() == true);
        REQUIRE(yaml::toValue("0x10").intValue() ==  16);
        REQUIRE(yaml::toValue("0x0").isIntValue() == true);
        REQUIRE(yaml::toValue("0x0").intValue() ==  0);

        REQUIRE(yaml::toValue("0x0001_0001").isIntValue() == true);
        REQUIRE(yaml::toValue("0x0001_0001").intValue() ==  65537);
    }

    SECTION("String is not int") {
        REQUIRE(yaml::toValue("hoge").isIntValue() == false);
        REQUIRE(yaml::toValue("10.54").isIntValue() == false);
        REQUIRE(yaml::toValue("12act").isIntValue() == false);
        REQUIRE(yaml::toValue("FALSE").isIntValue() == false);
    }

}

TEST_CASE("BOOL YAMLValue test", "") {
    std::vector<Value> bv_ts{
                yaml::toValue("TRUE"), 
                yaml::toValue("True"), 
                yaml::toValue("true"), 
                yaml::toValue("Yes"),
                yaml::toValue("Y")};
    std::vector<Value> bv_fs{
                yaml::toValue("FALSE"),
                yaml::toValue("False"),
                yaml::toValue("false"), 
                yaml::toValue("No"),
                yaml::toValue("N")};



    SECTION("Concat bool value access") {
        for(auto bv_t : bv_ts) {
            REQUIRE(bv_t.isBoolValue() == true);
            REQUIRE(Value::boolValue(bv_t)== true);
        }
        for(auto bv_f : bv_fs) {
            REQUIRE(bv_f.isBoolValue() == true);
            REQUIRE(Value::boolValue(bv_f)== false);
        }
    }

    SECTION("String is not bool") {
        REQUIRE(yaml::toValue("hoge").isBoolValue() == false);
        REQUIRE(yaml::toValue("10.54").isBoolValue() == false);
        REQUIRE(yaml::toValue("12act").isBoolValue() == false);
        REQUIRE(yaml::toValue("12").isBoolValue() == false);
    }
}

