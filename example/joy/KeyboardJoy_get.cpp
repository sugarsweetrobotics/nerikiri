#include "nerikiri/nerikiri.h"
#include "nerikiri/container_operation_factory.h"

#include "KeyboardJoy.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* KeyboardJoy_get() {
        return containerOperationFactory<KeyboardJoy>(
        {
          {"typeName", "get"},
          {"defaultArg", {
              {"arg", ""}
          }},
        },
        [](auto& container, auto arg) {
            if (!container.initialized) {
                return Value::error(("KeyboardJoy is not initialized."));
            }
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