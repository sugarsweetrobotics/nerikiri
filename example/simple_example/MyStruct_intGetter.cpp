#include "nerikiri/nerikiri.h"
#include "nerikiri/container_operation_factory.h"

#include "MyStruct.h"

using namespace nerikiri;

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