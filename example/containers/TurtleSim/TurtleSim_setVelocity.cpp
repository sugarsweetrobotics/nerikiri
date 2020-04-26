#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/containeroperationfactory.h"

#include "TurtleSim.h"

using namespace nerikiri;

extern "C" {

void* TurtleSim_setVelocity() {
    return containerOperationFactory<TurtleSim>(
    {
      {"name", "setVelocity"},
      {"defaultArg", {
          {"data", {
            {"name", "turtle0"},
            {"vx", 0.0},
            {"vy", 0.0},
            {"vth", 0.0}
          }}
      }},
    },
    [](auto& container, auto argdata) {
        std::cout << "TurtleSim_setVelocity()" << std::endl;
        auto& arg = argdata.at("data");
        if (!arg.hasKey("name")) {
            return Value::error("TurtleSim _setVelocity argument must have name argument.");
        }
        if (!(arg.hasKey("vx") && arg.hasKey("vy") && arg.hasKey("vth"))) {
            return Value::error("TurtleSim _setVelocity argument must have vx, vy, vth argument.");
        }

        auto it = container.turtles.begin();
        for(; it != container.turtles.end();++it) {
            if ((*it).name == arg.at("name").stringValue()) {
                (*it).vx = arg.at("vx").doubleValue();
                (*it).vy = arg.at("vy").doubleValue();
                (*it).vth = arg.at("vth").doubleValue();
                return Value(arg);
            }
        }
        return Value::error("TurtleSim_setVelocity failed. Name (" + arg.at("name").stringValue() + ") can not be found.");
    });
}

};