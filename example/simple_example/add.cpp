#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/operation.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION void* add() {
        return operationFactory({ {"typeName", "add"},
              {"defaultArg", {
                {"arg01", 0},
                {"arg02", 0}
              }}
            },
            [](auto arg) {
                return Value({ {"result", arg["arg01"].intValue() + arg["arg02"].intValue()} });
            });
    }
}