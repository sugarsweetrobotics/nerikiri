#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <iostream>
#include <nerikiri/nerikiri.h>
#include <nerikiri/http/client.h>
#include <nerikiri/http/server.h>
using namespace nerikiri;


SCENARIO( "HTTP Service", "[http]" ) {
  logger::setLogLevel(logger::TRACE);
  GIVEN("Without Service") {
    REQUIRE( http::get("localhost", 9955, "/").status == 0);
  }
  
  GIVEN("With Server") {
    int port = 9559;
    auto s = http::server("localhost", port);
    s = http::serve("/", "GET", [&](http::Request&& r) {
	return http::Response(200, "Hello");
      }, std::move(s));
    
    s = http::listen(1.0, std::move(s));
    
    THEN("Can access to server") {
      auto r = http::get("localhost", port, "/");
      REQUIRE(r.status == 200);
      REQUIRE(r.body == "Hello");
    }
  }
  
  GIVEN("With Server (functional style)") {
    int port = 9556;
    auto t = compose<http::Server_ptr>(
				       http::serve("/", "GET",
						   [](http::Request&& r) {
						     return http::Response(200, "Hello");
						   }),
				       http::listen(1.0));
    auto s = t(http::server("localhost", port));
    THEN("Can access to server") {
      auto r = http::get("localhost", port, "/");
      REQUIRE(r.status == 200);
      REQUIRE(r.body == "Hello");
    }
  }
  
  
  /*
    GIVEN("With Server") {
    WHEN("Server gives hello") {
      int port = 9555;
      auto s = http::server("localhost", port);
      REQUIRE(s->address == "localhost");
      REQUIRE(s->port == port);
    
      s = http::serve(std::move(s), "/", "GET", [&](http::Request&& r) {
	  return http::Response(200, "Hello");
	});
      s = http::listen(std::move(s), 1.0);
      THEN("Get Hello Message") {
	auto r = http::get("localhost", port, "/");
	REQUIRE(r.status == 200);
	REQUIRE(r.body == "Hello");
      }
    }


    WHEN("Server requests Hello") {
      int port = 9556;
      auto s = http::server("localhost", port);
      REQUIRE(s->address == "localhost");
      REQUIRE(s->port == port);
    
      s = http::serve(std::move(s), "/", "PUT", [&](http::Request&& r) {
	  return http::Response(200, r.body + " World");
	});
      s = http::listen(std::move(s), 1.0);

      THEN("Put Hello Message") {
	auto r = http::put("localhost", port, "/", "Hello", "text/plain");
	REQUIRE(r.status == 200);
	REQUIRE(r.body == "Hello World");
      }
      }*/
    
    /*
    WHEN("Server binds Dynamic Port") {
      int port = 0;
      auto s = http::server("localhost", port);
      REQUIRE(s->address == "localhost");
      REQUIRE(s->port == port);
    
      http::serve(s, "/", "GET", [&](http::Request&& r) {
	  return http::Response(200, "Hello");
	});
      port = http::listen(s, 1.0);
      std::cout << "port is " << port << std::endl;
      THEN("Get Hello Message") {
	auto r = http::get("localhost", port, "/");
	REQUIRE(r.status == 200);
	REQUIRE(r.body == "Hello");
      }
      }*/

  //  }

}


