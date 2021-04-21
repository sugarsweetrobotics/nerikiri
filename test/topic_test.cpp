
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch_all.hpp"
using namespace nerikiri;

#include "operations_for_tests.h"

SCENARIO( "Topic Operation test", "[operaion]" ) {



 GIVEN("Topic connectoin with Container operations") {
    const std::string jsonStr = R"(
{
  "logger": { "logLevel": "WARN" },
  "operations": {
        "precreate": [
            {
                "typeName": "zero",
                "fullName": "zero0.ope",
                "publish": "topic01"
            }
        ]
  },
  "containers": {
    "precreate": [
          {
            "typeName": "MyStruct",
            "fullName": "MyStruct0.ctn",
            "operations": [
              {
                "typeName": "set",
                "instanceName": "set0.ope",
                "subscribe": {"value": "topic01"}
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

    auto p = nerikiri::process("container_test", jsonStr);
      
    p->loadContainerFactory(cf0);
    p->loadContainerOperationFactory(copf0);
    p->loadContainerOperationFactory(copf1);
    p->loadContainerOperationFactory(copf2);
    p->loadOperationFactory(opf1);
    p->loadOperationFactory(opf2);
    p->loadOperationFactory(opf3);
    p->loadOperationFactory(opf4);
    p->startAsync();

    auto zero0ope = p->store()->get<OperationAPI>("zero0.ope");
    REQUIRE(zero0ope->isNull() == false);

    auto c = p->store()->get<ContainerAPI>("MyStruct0.ctn");
    REQUIRE(c->isNull() == false);

    auto getter = p->store()->get<OperationAPI>("MyStruct0.ctn:get0.ope");
    REQUIRE(getter->isNull() != true);
    auto setter = p->store()->get<OperationAPI>("MyStruct0.ctn:set0.ope");
    REQUIRE(setter->isNull() != true);

    THEN("Container is stanby") {
      REQUIRE(c->instanceName() == "MyStruct0.ctn");

      AND_THEN("Container operation test") {
        auto v = getter->call({});
        REQUIRE(Value::intValue(v) == 0);
        auto v2 = setter->call({{"value", 11}});
        auto v3 = getter->call({});
        REQUIRE(Value::intValue(v3) == 11);
      }

      AND_THEN("Connected") {
        auto v2 = setter->call({{"value", 11}});
        auto v3 = getter->call({});
        REQUIRE(Value::intValue(v3) == 11);

        zero0ope->execute();

        auto v4 = getter->call({});
        REQUIRE(Value::intValue(v4) == 0);
      }
    }

  }    


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

  GIVEN("Topic connection with Operation") {
    const std::string jsonStr = R"(
{
    "logger": { "logLevel": "WARN" },

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
