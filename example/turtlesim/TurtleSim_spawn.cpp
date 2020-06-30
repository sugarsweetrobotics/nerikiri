#include "nerikiri/nerikiri.h"
#include "nerikiri/containeroperationfactory.h"

#include "TurtleSim.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* TurtleSim_spawn() {
        return containerOperationFactory<TurtleSim>(
            {
              {"typeName", "spawn"},
              {"defaultArg", {
                  {"name", "turtle0"}
              }},
            },
            [](auto& container, auto arg) {
                Turtle turtle("turtle01");
                if (!arg.hasKey("name")) {
                    return Value::error("TurtleSim _spawn argument must have name argument.");
                }
                turtle.name = arg.at("name").stringValue();
                if (arg.hasKey("x") && arg.hasKey("y") && arg.hasKey("th")) {
                    turtle.x = arg.at("x").doubleValue();
                    turtle.y = arg.at("y").doubleValue();
                    turtle.th = arg.at("th").doubleValue();
                }
                container.turtles.emplace_back(turtle);
                return Value(arg);
            });
    }

}