#include <nerikiri/nerikiri.h>
#include <nerikiri/container.h>

#include "JoyController.h"

using namespace nerikiri;

extern "C" {
    NK_OPERATION  void* JoyController_get() {
        return containerOperationFactory<JoyController>(
        {
          {"typeName", "get"},
          {"defaultArg", {
              {}
          }},
        },
        [](auto& container, auto arg) {
            //std::cout << "JoyController_get()" << std::endl;
            if (container.gamepad) {
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