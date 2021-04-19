#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>



namespace nerikiri {

    struct Time {
        const long sec;
        const long nsec;
    };

    inline bool operator==(const Time& x0, const Time& x1) {
        return x0.sec == x1.sec && x0.nsec == x1.nsec;
    }

    inline bool operator!=(const Time& x0, const Time& x1) { return ! (x0 == x1); }

    inline bool operator>(const Time& x0, const Time& x1) {
        return ((x0.sec > x1.sec) ||  (x0.sec == x1.sec && x0.nsec > x1.nsec));
    }
    inline bool operator>=(const Time& x0, const Time& x1) {
        return ((x0.sec > x1.sec) || (x0.sec == x1.sec && x0.nsec >= x1.nsec));
    }

    inline bool operator<(const Time& x0, const Time& x1) { return ! (x0 >= x1); }
    inline bool operator<=(const Time& x0, const Time& x1) { return ! (x0 > x1); }

    struct Vector3D {
        const double x, y, z;
        Vector3D() : x(0), y(0), z(0) {}
        Vector3D(double _x, double _y, double _z): x(_x), y(_y), z(_z) {}
    };

    inline Vector3D operator+(const Vector3D& v0, const Vector3D& v1) {
        return {v0.x + v1.x, v0.y + v1.y, v0.z + v1.z};
    }

    inline Vector3D operator-(const Vector3D& v0, const Vector3D& v1) {
        return {v0.x - v1.x, v0.y - v1.y, v0.z - v1.z};
    }

    inline double norm(const Vector3D& v) {
        return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    }

    inline bool operator==(const Vector3D& v0, const Vector3D& v1) {
        return (v0.x == v1.x && v0.y == v1.y && v0.z == v1.z);
    }

    
    struct Quaternion {
        const double x, y, z, w;
        Quaternion() : x(0), y(0), z(0), w(1) {}
        Quaternion(double _x, double _y, double _z, double _w) : x(_x), y(_y), z(_z), w(_w) {}
    };

    std::string toStr(const Quaternion& q) {
        std::stringstream ss;
        ss << "Quaternion(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
        return ss.str();
    }

    inline double norm(const Quaternion& v) {
        return sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
    }

    inline Quaternion operator-(const Quaternion& q0, const Quaternion& q1) {
        return {q0.x - q1.x, q0.y - q1.y, q0.z - q1.z, q0.w - q1.w};
    }

    inline bool operator==(const Quaternion& q0, const Quaternion& q1) {
        return (q0.x == q1.x && q0.y == q1.y && q0.z == q1.z && q0.w == q1.w);
    }

    inline Quaternion dot(const Quaternion& q, const Quaternion& p) {
        return {
             q.w * p.x - q.z * p.y + q.y * p.z + q.x * p.w,
             q.z * p.x + q.w * p.y - q.x * p.z + q.y * p.w,
            -q.y * p.x + q.x * p.y + q.w * p.z + q.z * p.w,
            -q.x * p.x - q.y * p.y - q.z * p.z + q.w * p.w
        };
    }
    
    inline Vector3D vector3d(Quaternion&& q) {
        return {q.x, q.y, q.z};
    }

    inline Vector3D dot(const Quaternion& q, const Vector3D& v) {
        return vector3d(dot(dot(q, Quaternion(v.x, v.y, v.z, 0)), q));
    }

    using Point3D = Vector3D;
    using Orientation3D = Quaternion;

    struct Pose3D {
        const Point3D position;
        const Orientation3D orientation;
    };

    struct TimedPose3D {
        const Time tm;
        const Pose3D pose;
    };

    inline Pose3D dot(const Pose3D& x0, const Pose3D& x1) {
        return {dot(x0.orientation, x1.position) + x0.position, dot(x0.orientation, x1.orientation)};
    }

    inline TimedPose3D dot(const TimedPose3D& x0, const TimedPose3D& x1) {
        return {x0.tm >= x1.tm ? x0.tm : x1.tm, dot(x0.pose, x1.pose)};
    }

    inline double radians(const double degrees) { return degrees * M_PI / 180.0; }

    inline void _sincos(const double theta, double* sinTh, double* cosTh) {
    #ifdef __APPLE__
        __sincos(theta, sinTh, cosTh);
    #else
        *sinTh = std::sin(theta);
        *cosTh = std::cos(theta);
    #endif
    }

    Quaternion EulerXYZtoQuaternion(const Vector3D& v) {
        double cx, sx, cy, sy, cz, sz;
        _sincos(v.x / 2.0, &sx, &cx);
        _sincos(v.y / 2.0, &sy, &cy);
        _sincos(v.z / 2.0, &sz, &cz);

        return {
            sx * cy * cz - cx * sy * sz,
            cx * sy * cz + sx * cy * sz,
            cx * cy * sz - sx * sy * cz,
            cx * cy * cz + sx * sy * sz
        };
    }
}