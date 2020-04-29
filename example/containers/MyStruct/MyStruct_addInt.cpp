
#include "nerikiri/nerikiri.h"
#include "nerikiri/containeroperationfactory.h"

#include "MyStruct.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* MyStruct_addInt() {
        return containerOperationFactory<MyStruct>(
            {
              {"name", "addInt"},
              {"defaultArg", {{"data", 1}}}
            },
            [](auto& container, auto arg) {
                container.intValue += arg.at("data").intValue();
                return Value(container.intValue);
            });
    }
}