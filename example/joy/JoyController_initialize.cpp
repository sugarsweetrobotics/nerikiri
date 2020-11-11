#include "nerikiri/nerikiri.h"
#include "nerikiri/container_operation_factory.h"

#include "JoyController.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* JoyController_initialize() {
        return containerOperationFactory<JoyController>(
        {
          {"typeName", "initialize"},
          {"defaultArg", {
              {"arg", ""}
          }},
        },
        [](auto& container, auto arg) {
            container.gamepad = std::make_shared<ssr::aqua2::GamePad>(arg.at("arg").stringValue().c_str());
            return arg;
        });
    }

}