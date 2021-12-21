#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>

#include <juiz/value.h>

namespace juiz {

    struct Time {
        long sec;
        long nsec;

        Time(const long& sec_, const long& nsec_): sec(sec_), nsec(nsec_) {}
        Time(): sec(0), nsec(0) {}
        Time& operator=(const Time& t) {
            sec = t.sec; nsec = t.nsec;
            return *this;
        }
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
        double x, y, z;
        Vector3D() : x(0), y(0), z(0) {}
        Vector3D(double _x, double _y, double _z): x(_x), y(_y), z(_z) {}
        Vector3D& operator=(const Vector3D& v) {
            x = v.x; y = v.y; z = v.z;
            return *this;
        }
    };

    inline std::string toString(const Vector3D& v) {
        std::stringstream ss;
        ss << "Vector3(x=" << v.x << ", y=" << v.y << ", z=" << v.z << ")";
        return ss.str();
    }

    inline std::string toStr(const Vector3D& q) {
        std::stringstream ss;
        ss << "Vector3D(" << q.x << ", " << q.y << ", " << q.z << ")";
        return ss.str();
    }


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

    inline bool nearlyEqual(const Vector3D& v0, const Vector3D& v1, const double epsilon=1.0E-6) {
        return norm(v0-v1) < epsilon;
    }

    
    struct Quaternion {
        double x, y, z, w;
        Quaternion() : x(0), y(0), z(0), w(1) {}
        Quaternion(double _x, double _y, double _z, double _w) : x(_x), y(_y), z(_z), w(_w) {}
        Quaternion& operator=(const Quaternion& q) {
            x = q.x; y = q.y; z = q.z; w = q.w;
            return *this;
        }
    };

    inline std::string toString(const Quaternion& q) {
        std::stringstream ss;
        ss << "Quaternion(x=" << q.x << ", y=" << q.y << ", z=" << q.z << ", w=" << q.w << ")";
        return ss.str();
    }

    inline Quaternion conjugated(const Quaternion& q) {
        return {-q.x, -q.y, -q.z, q.w};
    }

    inline std::string toStr(const Quaternion& q) {
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

    inline bool nearlyEqual(const Quaternion& v0, const Quaternion& v1, const double epsilon=1.0E-6) {
        return norm(v0-v1) < epsilon;
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
        return vector3d(dot(q, dot(Quaternion(v.x, v.y, v.z, 0), conjugated(q))));
    }

    using Point3D = Vector3D;
    using Orientation3D = Quaternion;

    struct Pose3D {
        Point3D position;
        Orientation3D orientation;
        Pose3D(): position(), orientation() {}
        Pose3D(const Point3D& pos, const Orientation3D& ori): position(pos), orientation(ori) {}
        Pose3D& operator=(const Pose3D& p) {
            position = p.position;
            orientation = p.orientation;
            return *this;
        }
    };


    inline std::string toString(const Pose3D& pose) {
        std::stringstream ss;
        ss << "Pose3D(position=" << toString(pose.position) << ", orientation=" << toString(pose.orientation) << ")";
        return ss.str();
    }

    inline std::string toStr(const Pose3D& q) {
        std::stringstream ss;
        ss << "Pose3D(position=" << toStr(q.position) << ", orientation=" << toStr(q.orientation) << ")";
        return ss.str();
    }

    struct TimedPose3D {
        Time tm;
        Pose3D pose;
        TimedPose3D() {}
        TimedPose3D(const Time& tm_, const Pose3D& pose_): tm(tm_), pose(pose_) {}
        TimedPose3D& operator=(const TimedPose3D& tp) {
            tm = tp.tm;
            pose = tp.pose;
            return *this;
        }
    };

    inline Pose3D dot(const Pose3D& x0, const Pose3D& x1) {
        return {dot(x1.orientation, x1.position) + x0.position , dot(x0.orientation, x1.orientation)};
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

    inline Quaternion EulerXYZToQuaternion(const Vector3D& v) {
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

    inline Vector3D QuaternionToEulerXYZ(const Quaternion& q) {
        const double q0q0 = q.w * q.w;
        const double q0q1 = q.w * q.x;
        const double q0q2 = q.w * q.y;
        const double q0q3 = q.w * q.z;
        const double q1q1 = q.x * q.x;
        const double q1q2 = q.x * q.y;
        const double q1q3 = q.x * q.z;
        const double q2q2 = q.y * q.y;
        const double q2q3 = q.y * q.z;
        const double q3q3 = q.z * q.z;
        return {atan2(2.0 * (q2q3 + q0q1), q0q0 - q1q1 - q2q2 + q3q3),
            asin(2.0 * (q0q2 - q1q3)),
            atan2(2.0 * (q1q2 + q0q3), q0q0 + q1q1 - q2q2 - q3q3)};
    }

    inline Time toTime(const Value& v) {
        return {Value::intValue(v["sec"]), Value::intValue(v["nsec"])};
    }

    inline Point3D toPoint3D(const Value& value) {
        return {value["x"].doubleValue(), value["y"].doubleValue(), value["z"].doubleValue()};
    }

    inline Orientation3D toOrientation3D(const Value& value) {
        return {value["x"].doubleValue(), value["y"].doubleValue(),
                value["z"].doubleValue(), value["w"].doubleValue()};
    }

    inline Pose3D toPose3D(const Value& value) {
        return {toPoint3D(value["position"]), toOrientation3D(value["orientation"])};
    }


    inline TimedPose3D toTimedPose3D(const Value& value) {
        return {toTime(value["tm"]),
                toPose3D(value["pose"])};
    }

    inline Value toValue(const Time& tm) {
        return {{"sec", (int64_t)tm.sec},
                {"nsec", (int64_t)tm.nsec}};
    }

    inline Value toValue(const Point3D& position) {
        return {{"x", position.x},
                {"y", position.y},
                {"z", position.z}};
    }
    
    inline Value toValue(const Orientation3D& orientation) {
        return {{"x", orientation.x},
                {"y", orientation.y},
                {"z", orientation.z},
                {"w", orientation.w}};
    }

    inline Value toValue(const Pose3D& pose) {
        return {{"position", toValue(pose.position)},
                {"orientation", toValue(pose.orientation)}};
    }

    inline Value toValue(const TimedPose3D& pose) {
        return {
                {"tm", toValue(pose.tm)},
                {"pose", toValue(pose.pose)}
            };
    }
    


}