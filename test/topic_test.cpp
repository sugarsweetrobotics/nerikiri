
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch_all.hpp"
using namespace nerikiri;

#include "operations_for_tests.h"

SCENARIO( "Topic Operation test", "[operaion]" ) {
  GIVEN("Topic connection with the simplest description") {
    const std::string jsonStr = R"(
{
    "logger": { "logLevel": "OFF" },

    "operations": {
        "precreate": [
            {
                "typeName": "zero",
                "fullName": "zero0.ope",
                "publish": "topic01"
            }, 
            {
                "typeName": "inc",
                "fullName": "inc0.ope",
                "subscribe": {
                    "arg01": "topic01"
                }
            }
        ]
    }
}  
)";


    auto p = nerikiri::process("operation_test", jsonStr);
    p->loadOperationFactory(opf1);
    p->loadOperationFactory(opf2);
    p->loadOperationFactory(opf3);
    p->loadOperationFactory(opf4);
    
    p->startAsync();

    auto zero0ope = p->store()->get<OperationAPI>("zero0.ope");
    REQUIRE(zero0ope->isNull() == false);
    auto inc0ope = p->store()->get<OperationAPI>("inc0.ope");
    REQUIRE(inc0ope->isNull() == false);
    //auto one0ope = p->store()->get<OperationAPI>("one0.ope");
    //REQUIRE(one0ope->isNull() == false);
    //auto add0ope = p->store()->get<OperationAPI>("add0.ope");
    //REQUIRE(add0ope->isNull() == false);
    
    THEN("Connect with API") {
      auto con1 = zero0ope->outlet()->connections();
      REQUIRE(con1.size() == 1);

      auto con2 = inc0ope->inlet("arg01")->connections();
      REQUIRE(con2.size() == 1);


      REQUIRE(Value::intValue(inc0ope->execute(), -1) == 2);

      zero0ope->execute();

      REQUIRE(Value::intValue(inc0ope->execute(), -1) == 1);
    }
  }


  GIVEN("Topic connection with simple description") {
    const std::string jsonStr = R"(
{
    "logger": { "logLevel": "OFF" },

    "operations": {
        "precreate": [
            {
                "typeName": "zero",
                "fullName": "zero0.ope",
                "publish": [ "topic01" ]
            }, 
            {
                "typeName": "inc",
                "fullName": "inc0.ope",
                "subscribe": {
                    "arg01": ["topic01"]
                }
            }
        ]
    }
}  
)";


    auto p = nerikiri::process("operation_test", jsonStr);
    p->loadOperationFactory(opf1);
    p->loadOperationFactory(opf2);
    p->loadOperationFactory(opf3);
    p->loadOperationFactory(opf4);
    
    p->startAsync();

    auto zero0ope = p->store()->get<OperationAPI>("zero0.ope");
    REQUIRE(zero0ope->isNull() == false);
    auto inc0ope = p->store()->get<OperationAPI>("inc0.ope");
    REQUIRE(inc0ope->isNull() == false);
    //auto one0ope = p->store()->get<OperationAPI>("one0.ope");
    //REQUIRE(one0ope->isNull() == false);
    //auto add0ope = p->store()->get<OperationAPI>("add0.ope");
    //REQUIRE(add0ope->isNull() == false);
    
    THEN("Connect with API") {
      auto con1 = zero0ope->outlet()->connections();
      REQUIRE(con1.size() == 1);

      auto con2 = inc0ope->inlet("arg01")->connections();
      REQUIRE(con2.size() == 1);


      REQUIRE(Value::intValue(inc0ope->execute(), -1) == 2);

      zero0ope->execute();

      REQUIRE(Value::intValue(inc0ope->execute(), -1) == 1);
    }
  }

  GIVEN("Operation basic behavior") {
    const std::string jsonStr = R"(
{
    "logger": { "logLevel": "OFF" },

    "operations": {
        "precreate": [
            {
                "typeName": "zero",
                "fullName": "zero0.ope",
                "publish": [ {"fullName": "topic01"} ]
            }, 
            {
                "typeName": "inc",
                "fullName": "inc0.ope",
                "subscribe": {
                    "arg01": [
                        {"fullName": "topic01"}
                    ]
                }
            }
        ]
    }
}  
)";

    auto p = nerikiri::process("operation_test", jsonStr);
    p->loadOperationFactory(opf1);
    p->loadOperationFactory(opf2);
    p->loadOperationFactory(opf3);
    p->loadOperationFactory(opf4);
    
    p->startAsync();

    auto zero0ope = p->store()->get<OperationAPI>("zero0.ope");
    REQUIRE(zero0ope->isNull() == false);
    auto inc0ope = p->store()->get<OperationAPI>("inc0.ope");
    REQUIRE(inc0ope->isNull() == false);
    //auto one0ope = p->store()->get<OperationAPI>("one0.ope");
    //REQUIRE(one0ope->isNull() == false);
    //auto add0ope = p->store()->get<OperationAPI>("add0.ope");
    //REQUIRE(add0ope->isNull() == false);
    
    THEN("Connect with API") {
      auto con1 = zero0ope->outlet()->connections();
      REQUIRE(con1.size() == 1);

      auto con2 = inc0ope->inlet("arg01")->connections();
      REQUIRE(con2.size() == 1);


      REQUIRE(Value::intValue(inc0ope->execute(), -1) == 2);

      zero0ope->execute();

      REQUIRE(Value::intValue(inc0ope->execute(), -1) == 1);
    }
  }
}
