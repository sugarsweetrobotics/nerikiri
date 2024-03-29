
#include <iostream>
#include <juiz/logger.h>
#include <juiz/process.h>


#include "operations_for_tests.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
using namespace juiz;


SCENARIO( "Container test", "[container]" ) {

  GIVEN("Container transformation behavior with anchor") {
    const std::string jsonStr = R"(
{
  "logger": { "logLevel": "TRACE" },


  "containers": {
    "precreate": [
      {
        "typeName": "MyStruct",
        "fullName": "MyStruct0.ctn"
      },
      {
        "typeName": "MyStruct",
        "fullName": "MyStruct1.ctn"
      }
    ],
    "transformation": [
      {
        "typeName": "static",
        "offset": {
          "position": {
            "x": 1.0,
            "y": 0.0,
            "z": 0.0
          },
          "orientation": {
            "x": 0.0,
            "y": 0.0,
            "z": 0.0,
            "w": 1.0
          }
        },
        "from": {
          "typeName": "MyStruct",
          "fullName": "MyStruct0.ctn"
        },
        "to": {
          "typeName": "MyStruct",
          "fullName": "MyStruct1.ctn"
        }
      }
    ]
  },

  "anchors": [
    {
      "fullName": "test_anchor",
      "typeName": "PeriodicAnchor",
      "rate": 10,
      "target": {
        "fullName": "MyStruct0.ctn"
      },
      "offset": {
        "position": {
          "x": 1.0,
          "y": 0.0,
          "z": 0.0
        },
        "orientation": {
          "x": 0.0,
          "y": 0.0,
          "z": 0.0,
          "w": 1.0
        }
      }
    }
  ]

  
})";

    auto p = juiz::process("container_test", jsonStr);
    
    p->load(cf0);
    p->load(copf0);
    p->load(copf1);
    p->load(copf2);
      
    p->startAsync();
      
    THEN("Anchor is ready") {
      auto a = p->store()->get<ContainerAPI>("test_anchor");
      REQUIRE(a->isNull() == false);
    }

    THEN("Container is stanby") {
      auto c = p->store()->get<ContainerAPI>("MyStruct0.ctn");
      REQUIRE(c->isNull() == false);
      REQUIRE(c->instanceName() == "MyStruct0.ctn");


      auto a = p->store()->get<ContainerAPI>("test_anchor");
      auto sop = a->operation("get_state.ope");
      REQUIRE(sop->isNull() == false);
      auto stat = sop->call({});
      REQUIRE(stat.stringValue() == "stopped");

      auto op = a->operation("activate_state_started.ope");
      REQUIRE(op->isNull() == false);
      op->execute();

      auto stat2 = sop->call({});
      REQUIRE(stat2.stringValue() == "started");

      AND_THEN("Container operation test") {
        auto cops = p->store()->list<OperationAPI>();
        /// REQUIRE(cops.size() == 5);

        auto getter = p->store()->get<OperationAPI>("MyStruct1.ctn:container_get_pose.ope");
        REQUIRE(getter->isNull() != true);
        auto setter = p->store()->get<OperationAPI>("MyStruct0.ctn:container_set_pose.ope");
        REQUIRE(setter->isNull() != true);
        auto v = getter->call({});
        REQUIRE(v.isObjectValue() == true);
        auto p = toTimedPose3D(v);
        //REQUIRE(p.pose.position.x == 0.0);
        //auto v2 = setter->execute();
        /*
        call({
          {"pose", toValue(TimedPose3D{Time{}, Pose3D{Point3D{}, Orientation3D{}}})}
        }); */

        auto v3 = getter->call({});
        REQUIRE(Value::doubleValue(v3["pose"]["position"]["x"]) == 2.0);
      }
    }
    p->stop();
  }

  GIVEN("Container basic behavior") {
    const std::string jsonStr = R"(
{
  "logger": { "logLevel": "WARN" },

  "containers": {
    "precreate": [
          {
            "typeName": "MyStruct",
            "instanceName": "MyStruct0.ctn",
            "operations": [
              {
                "typeName": "set",
                "instanceName": "set0.ope"
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

    auto p = juiz::process("container_test", jsonStr);
    
    p->load(cf0);
    p->load(copf0);
    p->load(copf1);
    p->load(copf2);
      
    p->startAsync();
      
    
    THEN("Container is stanby") {
      auto c = p->store()->get<ContainerAPI>("MyStruct0.ctn");
      REQUIRE(c->isNull() == false);
      REQUIRE(c->instanceName() == "MyStruct0.ctn");

      AND_THEN("Container operation test") {
        auto cops = p->store()->list<OperationAPI>();
        REQUIRE(cops.size() == 5);

        auto getter = p->store()->get<OperationAPI>("MyStruct0.ctn:get0.ope");
        REQUIRE(getter->isNull() != true);
        auto setter = p->store()->get<OperationAPI>("MyStruct0.ctn:set0.ope");
        REQUIRE(setter->isNull() != true);
        auto v = getter->call({});
        REQUIRE(Value::intValue(v) == 0);
        auto v2 = setter->call({{"value", 11}});

        auto v3 = getter->call({});
        REQUIRE(Value::intValue(v3) == 11);
      }
    }
    p->stop();
  }

  GIVEN("Container transformation behavior") {
    const std::string jsonStr = R"(
{
  "logger": { "logLevel": "WARN" },

  "containers": {
    "precreate": [
      {
        "typeName": "MyStruct",
        "fullName": "MyStruct0.ctn"
      },
      {
        "typeName": "MyStruct",
        "fullName": "MyStruct1.ctn"
      }
    ],
    "transformation": [
      {
        "typeName": "static",
        "offset": {
          "position": {
            "x": 1.0,
            "y": 0.0,
            "z": 0.0
          },
          "orientation": {
            "x": 0.0,
            "y": 0.0,
            "z": 0.0,
            "w": 1.0
          }
        },
        "from": {
          "typeName": "MyStruct",
          "fullName": "MyStruct0.ctn"
        },
        "to": {
          "typeName": "MyStruct",
          "fullName": "MyStruct1.ctn"
        }
      }
    ]
  }
})";

    auto p = juiz::process("container_test", jsonStr);
    
    p->load(cf0);
    p->load(copf0);
    p->load(copf1);
    p->load(copf2);
      
    p->startAsync();
      
    
    THEN("Container is stanby") {
      auto c = p->store()->get<ContainerAPI>("MyStruct0.ctn");
      REQUIRE(c->isNull() == false);
      REQUIRE(c->instanceName() == "MyStruct0.ctn");

      AND_THEN("Container operation test") {
        auto cops = p->store()->list<OperationAPI>();
        REQUIRE(cops.size() == 5);

        auto getter = p->store()->get<OperationAPI>("MyStruct1.ctn:container_get_pose.ope");
        REQUIRE(getter->isNull() != true);
        auto setter = p->store()->get<OperationAPI>("MyStruct0.ctn:container_set_pose.ope");
        REQUIRE(setter->isNull() != true);
        auto v = getter->call({});
        REQUIRE(v.isObjectValue() == true);
        auto p = toTimedPose3D(v);
        REQUIRE(p.pose.position.x == 0.0);
        auto v2 = setter->execute();
        /*
        call({
          {"pose", toValue(TimedPose3D{Time{}, Pose3D{Point3D{}, Orientation3D{}}})}
        }); */

        auto v3 = getter->call({});
        REQUIRE(Value::doubleValue(v3["pose"]["position"]["x"]) == 1.0);
      }
    }
    p->stop();
  }
}
