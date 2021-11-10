#include <juiz/juiz.h>
#include <juiz/operation.h>
#include <juiz/geometry.h>

#include "dynamic_transformation_operation.h"

using namespace juiz;

JUIZ_OPERATION void* dynamic_transformation_operation() {
    return operationFactory(
    {
        {"typeName", "dynamic_transformation_operation"},
        {"description", "Dynamic Transformation between Containers"},
        {"defaultArg", Value({

            {"pose", {
                {"tm" , {
                    {"sec", {0}},
                    {"nsec", {0}}
                }},
                {"pose", {
                    {"position", {
                        {"x", 0.0},
                        {"y", 0.0},
                        {"z", 0.0}
                    }},
                    {"orientation", {
                        {"x", 0.0},
                        {"y", 0.0},
                        {"z", 0.0},
                        {"w", 1.0}
                    }}
                }}
            }},
            {"input", {
                {"position", {
                    {"x", 0.},
                    {"y", 0.},
                    {"z", 0.}
                }},
                {"orientation", {
                    {"x", 0.},
                    {"y", 0.},
                    {"z", 0.},
                    {"w", 1.}
                }}
            }}
        }),
    }},
    [](auto arg) {
        auto pose = toTimedPose3D(arg["pose"]);
        auto offset = toPose3D(arg["input"]);
        auto result = dot(pose.pose, offset);
        return toValue(TimedPose3D(pose.tm, result));
    });
}

