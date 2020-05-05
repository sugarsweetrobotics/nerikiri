#include "nerikiri/nerikiri.h"
#include "nerikiri/containers/containeroperationfactory.h"

#include "MyStruct.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* MyStruct_intGetter() {
        return containerOperationFactory<MyStruct>(
            {
              {"name", "intGetter"},
              {"defaultArg", {}},
            },
            [](auto& container, auto arg) {
                return Value(container.intValue);
            });
    }

}