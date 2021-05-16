#include <juiz/nerikiri.h>
#include <juiz/container.h>
#include "KeyboardJoy.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* KeyboardJoy_initialize() {
        return containerOperationFactory<KeyboardJoy>(
        {
          {"typeName", "initialize"},
          {"defaultArg", {
              {"arg", ""}
          }},
        },
        [](auto& container, auto arg) {
            ::initialize(container);
            return arg;
        });
    }

}