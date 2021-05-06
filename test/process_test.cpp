
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch_all.hpp"

#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"


#include "operations_for_tests.h"

using namespace nerikiri;

SCENARIO( "Process test", "[process]" ) {
  GIVEN("Process basic behavior") {
    const std::string jsonStr1 = R"(
{
    "logger": { "logLevel": "WARN" },

    "operations": {
        "precreate": [
            {
                "typeName": "inc",
                "fullName": "inc0.ope"
            }
        ]
    }, 
    "brokers": {
      "load_paths": ["lib", "lib/Debug", "build/lib", "build/lib/Debug"],
      "preload": ["HTTPBroker"],
      "precreate": [
        {
            "typeName": "HTTPBroker",
            "port": 8080,
            "host": "0.0.0.0",
            "fullName": "HTTPBroker0.brk"
        }
      ]
    }
}  
)";

    const std::string jsonStr2 = R"(
{
    "logger": { "logLevel": "WARN" },

    "operations": {
        "precreate": [
            {
                "typeName": "zero",
                "fullName": "zero0.ope"
            }
        ]
    }, 
    "brokers": {
      "load_paths": ["lib", "lib/Debug", "build/lib", "build/lib/Debug"],
      "preload": ["HTTPBroker"],
      "precreate": [
        {
            "typeName": "HTTPBroker",
            "port": 8081,
            "host": "0.0.0.0",
            "fullName": "HTTPBroker0.brk"
        }
      ]
    }
}  
)";

    auto p1 = nerikiri::process("process_1", jsonStr1);
    p1->loadOperationFactory(opf1);
    p1->loadOperationFactory(opf2);
    p1->loadOperationFactory(opf3);
    p1->startAsync();


    auto p2 = nerikiri::process("process_2", jsonStr2);

    p2->loadOperationFactory(opf1);
    p2->loadOperationFactory(opf2);
    p2->loadOperationFactory(opf3);
    p2->startAsync();


    auto inc0ope = p1->store()->get<OperationAPI>("inc0.ope");
    REQUIRE(inc0ope->isNull() == false);

    auto inc0opeInfo = inc0ope->info();
    auto brk1 = p1->store()->broker("HTTPBroker0.brk");
    REQUIRE(brk1->isNull() == false);
    inc0opeInfo["broker"] = brk1->fullInfo();
    auto inc0proxy = p1->store()->operationProxy(inc0opeInfo);
    REQUIRE(inc0proxy->isNull() == false);

    auto zero0ope = p2->store()->get<OperationAPI>("zero0.ope");
    REQUIRE(zero0ope->isNull() == false);
    
    auto zero0opeInfo = zero0ope->info();
    auto brk2 = p2->store()->broker("HTTPBroker0.brk");
    REQUIRE(brk2->isNull() == false);
    zero0opeInfo["broker"] = brk2->fullInfo();
    auto zero0proxy = p1->store()->operationProxy(zero0opeInfo);
    REQUIRE(zero0proxy->isNull() == false);

    REQUIRE(inc0ope->inlet("arg01")->isNull() == false);
    WHEN("Connect with API") {
      auto ret = nerikiri::connect(p1->store()->brokerFactory("CoreBroker")->createProxy(""), "con0", inc0proxy->inlet("arg01"), zero0proxy->outlet());
      REQUIRE(ret.isError() == false);

      auto con1 = zero0ope->outlet()->connections();
      REQUIRE(con1.size() == 1);
      REQUIRE(con1[0]->fullName() == "con0");

      auto con12 = zero0proxy->outlet()->connections();
      REQUIRE(con12.size() == 1);
      REQUIRE(con12[0]->fullName() == "con0");

      auto con2 = inc0ope->inlet("arg01")->connections();
      REQUIRE(con2.size() == 1);
      REQUIRE(con2[0]->fullName() == "con0");

        operationIncIsCalled = false;
      REQUIRE(Value::intValue(inc0ope->execute(), -1) == 2);
        REQUIRE(operationIncIsCalled == true);
        
        
        operationIncIsCalled = false;
        operationZeroIsCalled = false;
      zero0ope->execute();
        REQUIRE(operationIncIsCalled == true);
        REQUIRE(operationZeroIsCalled == true);

      REQUIRE(Value::intValue(inc0ope->execute(), -1) == 1);
    }
    p1->stop();
    p2->stop();
  }
}
