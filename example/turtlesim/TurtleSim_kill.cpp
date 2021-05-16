#include <juiz/nerikiri.h>
#include <juiz/container.h>

#include "TurtleSim.h"

using namespace juiz;

extern "C" {

    NK_OPERATION  void* TurtleSim_kill() {
        return containerOperationFactory<TurtleSim>(
            {
              {"typeName", "kill"},
              {"defaultArg", {
                  {"name", "turtle0"}
              }},
            },
            [](auto& container, auto arg) {
                if (!arg.hasKey("name")) {
                    return Value::error("TurtleSim _kill argument must have name argument.");
                }
                auto it = container.turtles.begin();
                for (; it != container.turtles.end(); ++it) {
                    if ((*it).name == arg.at("name").stringValue()) {
                        container.turtles.erase(it);
                        return Value(arg);
                    }
                }
                return Value::error("TurtleSim_kill failed. Name (" + arg.at("name").stringValue() + ") can not be found.");
            });
    }

}