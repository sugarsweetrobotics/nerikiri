
#include <juiz/juiz.h>
#include <juiz/container.h>

#include "MyStruct.h"

using namespace juiz;

extern "C" {

    JUIZ_OPERATION  void* MyStruct_addInt() {
        return containerOperationFactory<MyStruct>(
            {
              {"typeName", "addInt"},
              {"defaultArg", {{"data", 1}}}
            },
            [](auto& container, auto arg) {
                container.intValue += arg.at("data").intValue();
                return Value(container.intValue);
            });
    }
}