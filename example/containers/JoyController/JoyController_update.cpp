#include "nerikiri/nerikiri.h"
#include "nerikiri/containers/containeroperationfactory.h"

#include "JoyController.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* JoyController_update() {
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