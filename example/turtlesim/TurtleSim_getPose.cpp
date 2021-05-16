#include <juiz/nerikiri.h>
#include <juiz/container.h>

#include "TurtleSim.h"

using namespace juiz;


extern "C" {

    NK_OPERATION  void* TurtleSim_getPose() {
        return containerOperationFactory<TurtleSim>(
            {
              {"typeName", "getPose"},
              {"defaultArg", {
                  {"name", "turtle0"}
              }},
            },
            [](auto& container, auto arg) {
                if (!arg.hasKey("name")) {
                    return Value::error("TurtleSim_getPose argument must have name argument.");
                }
                auto it = container.turtles.begin();
                for (; it != container.turtles.end(); ++it) {
                    if ((*it).name == arg.at("name").stringValue()) {
                        return Value{
                            {"x", (*it).x}, {"y", (*it).y}, {"th", (*it).th},
                            {"vx", (*it).vx}, {"vy", (*it).vy}, {"vth", (*it).vth}
                        };
                    }
                }
                return Value::error("TurtleSim_getPose failed. Name (" + arg.at("name").stringValue() + ") can not be found.");
            });
    }

}