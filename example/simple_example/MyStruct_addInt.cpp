
#include "nerikiri/nerikiri.h"
#include "nerikiri/container.h"

#include "MyStruct.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* MyStruct_addInt() {
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