#include "nerikiri/nerikiri.h"
#include "nerikiri/containeroperationfactory.h"

#include "MyStruct.h"

using namespace nerikiri;

extern "C" {
    void* MyStruct_intGetter();
};


void* MyStruct_intGetter() {
    return containerOperationFactory<MyStruct>(
    {
      {"name", "intGetter"},
      {"defaultArg", {}},
    },
    [](auto& container, auto arg) {
      return Value(container.intValue);
    });
}