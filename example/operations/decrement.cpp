#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/operationfactory.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION void* decrement() {
        return operationFactory({ {"typeName", "decrement"},
              {"defaultArg", {
                {"arg01", 0}
              }}
            },
            [](auto arg) {
                return Value(arg["arg01"].intValue() - 1);
            });
    }
}