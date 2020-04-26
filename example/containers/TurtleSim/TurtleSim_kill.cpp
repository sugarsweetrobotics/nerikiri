#include "nerikiri/nerikiri.h"
#include "nerikiri/containeroperationfactory.h"

#include "TurtleSim.h"

using namespace nerikiri;

extern "C" {

void* TurtleSim_kill() {
    return containerOperationFactory<TurtleSim>(
    {
      {"name", "kill"},
      {"defaultArg", {
          {"name", "turtle0"}
      }},
    },
    [](auto& container, auto arg) {
        if (!arg.hasKey("name")) {
            return Value::error("TurtleSim _kill argument must have name argument.");
        }
        auto it = container.turtles.begin();
        for(; it != container.turtles.end();++it) {
            if ((*it).name == arg.at("name").stringValue()) {
                container.turtles.erase(it);
                return Value(arg);
            }
        }
        return Value::error("TurtleSim_kill failed. Name (" + arg.at("name").stringValue() + ") can not be found.");
    });
}

};