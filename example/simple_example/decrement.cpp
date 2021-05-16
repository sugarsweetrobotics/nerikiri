#include <iostream>

#include <juiz/juiz.h>
#include <juiz/operation.h>

using namespace juiz;

extern "C" {

    JUIZ_OPERATION void* decrement() {
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