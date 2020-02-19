
#include <iostream>
#include <nerikiri/nerikiri.h>
#include <nerikiri/http/httpbroker.h>


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
using namespace nerikiri;


SCENARIO( "HTTP Service", "[http]" ) {
  logger::setLogLevel(logger::TRACE);
  GIVEN("Broker Composition") {
    Process p("process_test");
      p.addOperation({{"increment"},
		     [](auto arg) { 
		       arg["arg01"] = arg["arg01"].intValue() + 1;
		       return arg;
		     }})
      .addBroker(nerikiri::Broker_ptr(new http::HTTPBroker()));

    //BrokerDictionary bd;
    //bd.add(nerikiri::Broker_ptr(new http::HTTPBroker()));
    //THEN("Process info") {
      //auto infos = p.getOperationInfos();
      
    //    }
  }
  
}
