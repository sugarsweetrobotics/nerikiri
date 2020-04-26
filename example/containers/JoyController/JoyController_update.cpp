#include "nerikiri/nerikiri.h"
#include "nerikiri/containeroperationfactory.h"

#include "JoyController.h"

using namespace nerikiri;

extern "C" {

void* JoyController_update() {
    return containerOperationFactory<JoyController>(
    {
      {"name", "update"},
      {"defaultArg", {
          {}
      }},
    },
    [](auto& container, auto arg) {
        if (container.gamepad) {
            container.gamepad->update();
        }
        return Value(arg);
    });
}

}