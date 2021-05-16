#include <juiz/juiz.h>
#include <juiz/container.h>
#include "KeyboardJoy.h"

using namespace juiz;

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