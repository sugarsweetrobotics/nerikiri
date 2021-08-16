#include <juiz/juiz.h>
#include <juiz/container.h>

#include "TurtleSim.h"

using namespace juiz;


extern "C" {

    JUIZ_OPERATION  void* TurtleSim_odomTransform() {
        return containerOperationFactory<TurtleSim>(
            {
              {"typeName", "odomTransform"},
              {"defaultArg", {
                    {"name", "turtle0"},
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
                    }}
              }},
            },
            [](auto& container, auto arg) {
                if (!arg.hasKey("name")) {
                    return Value::error("TurtleSim_odomTransform argument must have name argument.");
                }
                auto it = container.turtles.begin();
                for (; it != container.turtles.end(); ++it) {
                    if ((*it).name == arg.at("name").stringValue()) {
                        auto odom = Pose3D({(*it).x, (*it).y, 0.0}, 
                            juiz::EulerXYZToQuaternion({0, 0, (*it).th}));
                        auto timedPose = toTimedPose3D(arg["pose"]);
                        auto output = dot(timedPose.pose, odom);
                        return toValue(TimedPose3D(timedPose.tm, output));
                    }
                }
                return Value::error("TurtleSim_getPose failed. Name (" + arg.at("name").stringValue() + ") can not be found.");
            });
    }

}