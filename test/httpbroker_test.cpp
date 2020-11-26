
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/process.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;

#include "operations_for_tests.h"


SCENARIO( "Broker test", "[broker]" ) {

  GIVEN("HTTPBroker basic behavior") {
    const std::string jsonStr = R"(
    {
        "logger": { 
          "logLevel": "TRACE"
        },

        "operations": {
          "precreate": [
            {"typeName": "inc", "instanceName": "inc0.ope"},
            {"typeName": "zero", "instanceName": "zero0.ope"}
          ]
        },

        "brokers": {
            "load_paths": ["test", "build/test", "../build/lib", "../build/example"],
            "preload": ["HTTPBroker"],
            "precreate": [
                {
                    "typeName": "HTTPBroker",
                    "port": 8080,
                    "host": "0.0.0.0",
                    "instanceName": "HTTPBroker0.brk"
                }
            ]
        }
    }  
    )";

    Process p("httpbroker_test", jsonStr);

    p.loadOperationFactory(opf1);
    p.loadOperationFactory(opf2);

    p.startAsync();
    REQUIRE(p.isRunning() == true);
    WHEN("HTTPBroker is running") {
      auto i = p.fullInfo();
      REQUIRE(i.isError() == false);

      REQUIRE(i.at("brokers").listValue().size() == 1);

      auto proxy = p.store()->brokerFactory("HTTPBroker")->createProxy({{"host", "localhost"}, {"port", 8080}});
      REQUIRE(proxy->isNull() == false);


      THEN("Process Info includes broker") {
        auto pInfo = proxy->getProcessInfo();
        REQUIRE(pInfo.isError() == false);
        REQUIRE(Value::string(pInfo.at("fullName")) == "httpbroker_test");
      }

      THEN("HTTPBroker Operation") {
        auto ope1 = p.store()->operation("zero0.ope");
        REQUIRE(ope1->isNull() == false);
        REQUIRE(Value::intValue(ope1->call({}), -1) == 0);

        auto ope2 = p.store()->operation("inc0.ope");
        REQUIRE(ope2->isNull() == false);
        REQUIRE(Value::intValue(ope2->call({{"arg01", 3}}), -1) == 4);
      }

      THEN("HTTPBroker Connection") {
        auto ope1 = p.store()->operation("zero0.ope");
        REQUIRE(ope1->isNull() == false);
        REQUIRE(Value::intValue(ope1->call({}), -1) == 0);

        auto ope2 = p.store()->operation("inc0.ope");
        REQUIRE(ope2->isNull() == false);
        REQUIRE(Value::intValue(ope2->call({{"arg01", 3}}), -1) == 4);

        Value conInfo{
          {"name", "con0"},
          {"type", "event"},
          {"broker", {
              {"typeName", "HTTPBroker"},
              {"host", "127.0.0.1"},
              {"port", "8080"}
            }
          },
          {"inlet", {
            {"name", "arg01"}, 
            {"operation", {
              {"fullName", "inc0.ope"},
              {"broker", {
                {"typeName", "HTTPBroker"},
                {"host", "127.0.0.1"},
                {"port", 8080}
              }}
            }}
          }},
          {"outlet", {
            {"operation", {
              {"fullName", "zero0.ope"},
              {"broker", {
                {"typeName", "HTTPBroker"},
                {"host", "127.0.0.1"},
                {"port", 8080}
              }}
            }}
          }}
        };

        proxy->connection()->createConnection(conInfo);


        auto con1 = ope1->outlet()->connections();
        REQUIRE(con1.size() == 1);

        auto con2 = ope2->inlet("arg01")->connections();
        REQUIRE(con2.size() == 1);


        REQUIRE(Value::intValue(ope2->execute(), -1) == 2);

        ope1->execute();

        REQUIRE(Value::intValue(ope2->execute(), -1) == 1);

      }
    }
    p.stop();
  }
}
