
#include <iostream>
#include "nerikiri/logger.h"
#include "nerikiri/process.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;

SCENARIO( "Broker test", "[broker]" ) {

  GIVEN("HTTPBroker basic behavior") {
    const std::string jsonStr = R"(
    {
        "logger": { 
          "logLevel": "ERROR"
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
    p.startAsync();
    REQUIRE(p.isRunning() == true);
    WHEN("HTTPBroker is running") {
      auto i = p.fullInfo();
      REQUIRE(i.isError() == false);

      REQUIRE(i.at("brokers").listValue().size() == 1);

      auto proxy = p.store()->brokerFactory("HTTPBroker")->createProxy({{"host", "localhost"}, {"port", 8080}});
      REQUIRE(proxy->isNull() == false);


      THEN("Process Info includes broker") {

      }
    }
    p.stop();
  }
}