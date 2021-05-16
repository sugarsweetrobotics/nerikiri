#include <iostream>

#include <juiz/juiz.h>
#include <juiz/operation.h>

using namespace juiz;

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