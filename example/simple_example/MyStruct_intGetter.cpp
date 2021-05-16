#include <juiz/nerikiri.h>
#include <juiz/container.h>

#include "MyStruct.h"

using namespace juiz;

extern "C" {

    NK_OPERATION  void* MyStruct_intGetter() {
        return containerOperationFactory<MyStruct>(
            {
              {"typeName", "intGetter"},
              {"defaultArg", {}},
            },
            [](auto& container, auto arg) {
                return Value(container.intValue);
            });
    }

}