#include <juiz/juiz.h>
#include <juiz/container.h>

#include "TurtleSim.h"

using namespace juiz;

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