#include <iostream>
#include <memory>
#include "nerikiri/geometry.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
using namespace nerikiri;




SCENARIO( "Geometry test", "[ec]" ) {
  GIVEN("Geometry basic behavior") {
    Quaternion q;
    REQUIRE(q == Quaternion(0, 0, 0, 1));
    Vector3D v;
    REQUIRE(v == Vector3D(0, 0, 0));

    Vector3D v0(1, 2, 3);
    Vector3D v1(4, 5, 6);
    Vector3D v2 = v0 + v1;
    REQUIRE(norm(v2 - Vector3D(5, 7, 9)) <= 0.00000001);
  }

  GIVEN("Quaternion and RPY") {
    auto q = EulerXYZtoQuaternion({radians(30.0), 0, 0});
    std::cout << toStr(q);
    REQUIRE(norm(q - Quaternion(0.258819, 0, 0, 0.9659258)) <= 0.0000002);
  }
}