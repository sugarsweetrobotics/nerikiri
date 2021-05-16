#include <juiz/juiz.h>
#include <juiz/container.h>
#include <iostream>
#include "JoyController.h"

using namespace juiz;

extern "C" {

    NK_OPERATION  void* JoyController_update() {
        return containerOperationFactory<JoyController>(
        {
          {"typeName", "update"},
          {"defaultArg", {
              {}
          }},
        },
        [](auto& container, auto arg) {
            // std::cout << "JoyController_update called" << std::endl;
            if (container.gamepad) {
                container.gamepad->update();
                auto v = Value(container.gamepad->buttons);
                return Value(
                    {
                        {"axis", Value(container.gamepad->axis)},
                        {"buttons", std::move(v) }
                    }
                );
            } 
            return Value::error(("Gamepad is not initialized."));
        });
    }

}