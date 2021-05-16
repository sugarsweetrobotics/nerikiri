#include <juiz/nerikiri.h>
#include <juiz/container.h>

#include "KeyboardJoy.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* KeyboardJoy_update() {
        return containerOperationFactory<KeyboardJoy>(
        {
          {"typeName", "update"},
          {"defaultArg", {
              {"arg", ""}
          }},
        },
        [](auto& container, auto arg) {
            if (!container.initialized) {
                return Value::error(("KeyboardJoy is not initialized."));
            }
            update(container);
            auto v = Value(container.buttons);
            return Value(
                {
                    {"axis", Value(container.axis)},
                    {"buttons", std::move(v) }
                }
            );
        });
    }

}