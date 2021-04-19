#include <iostream>
#include <memory>
#include "nerikiri/geometry.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
using namespace nerikiri;


SCENARIO("Pose test", "[pose]") {
  GIVEN("Pose basic behaviour") {
    const Pose3D p0({0, 1, 2}, {});
    const Pose3D p1({0, 2, 3}, {});

    const auto p3 = dot(p0, p1);
    REQUIRE(nearlyEqual(p3.position, Vector3D{0,3,5}));
  }

  GIVEN("Pose rotation behaviour") {
    const Pose3D p0({1, 0, 0}, {});
    const Pose3D p1({1, 0, 0}, EulerXYZToQuaternion({0, 0, radians(90)}));

    const auto p3 = dot(p0, p1);
    std::cout << toStr(p3) << std::endl;;
    REQUIRE(nearlyEqual(p3.position, Vector3D{1, 1, 0}));
  }

  GIVEN("Pose rotation behaviour") {
    const Pose3D p0({0, 0, 1}, {});
    const Pose3D p1({0, 0, 1}, EulerXYZToQuaternion({0, 0, radians(90)}));

    const auto p3 = dot(p0, p1);
    //std::cout << toStr(p3) << std::endl;;
    REQUIRE(nearlyEqual(p3.position, Vector3D{0, 0, 2}));
  }

  GIVEN("Pose rotation behaviour") {
    const Pose3D p0({0, 0, 1}, {});
    const Pose3D p1({0, 0, 1}, EulerXYZToQuaternion({0, radians(90), 0}));

    const auto p3 = dot(p0, p1);
    //std::cout << toStr(p3) << std::endl;;
    REQUIRE(nearlyEqual(p3.position, Vector3D{1, 0, 1}));
  }

}

SCENARIO( "Geometry test", "[geometry]" ) {

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
    const auto q1 = EulerXYZToQuaternion({radians(30.0), 0, 0});
    REQUIRE(norm(q1 - Quaternion(0.258819, 0, 0, 0.9659258)) <= 0.0000002);
    const auto v1 = QuaternionToEulerXYZ(q1);
    REQUIRE(norm(v1 - Vector3D{radians(30.0), 0, 0}) <= 0.0000002);

    const auto q2 = EulerXYZToQuaternion({ 0, radians(30.0), 0});
    REQUIRE(norm(q2 - Quaternion(0, 0.2588190, 0, 0.9659258)) <= 0.0000002);
    const auto v2 = QuaternionToEulerXYZ(q2);
    REQUIRE(norm(v2 - Vector3D{0, radians(30.0), 0}) <= 0.0000002);

    const auto q3 = EulerXYZToQuaternion({ 0, 0, radians(30.0)});
    REQUIRE(norm(q3 - Quaternion(0, 0, 0.258819, 0.9659258)) <= 0.0000002);
    const auto v3 = QuaternionToEulerXYZ(q3);
    REQUIRE(norm(v3 - Vector3D{0, 0, radians(30.0)}) <= 0.0000002);

    THEN("Orientaion Rotation") {
        const auto q1 = EulerXYZToQuaternion({radians(30.0), 0, 0});
        const auto q2 = EulerXYZToQuaternion({0, radians(30.0), 0});
        const auto q3 = dot(q2, q1);
        REQUIRE(norm(q3 - EulerXYZToQuaternion({radians(30.0), radians(30.0), 0})) <= 0.0000002);
    }

    THEN("Vector Rotation") {
        const Vector3D v1(1, 0, 0);
        const auto q1 = EulerXYZToQuaternion({0, 0, radians(90.0)});
        const auto v2 = dot(q1, v1);
        std::cout << "v2:" << toStr(v2) << "/ q1:" << toStr(q1) << std::endl;
        REQUIRE(nearlyEqual(v2, Vector3D{0, 1, 0}));
    }
  }
}