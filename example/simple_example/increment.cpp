#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/operation.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* increment() {
        return operationFactory(
            {
                {"typeName", "increment"},
                {"defaultArg", {
                    {"data", 0}
                }}
            },
            [](auto arg) {
                std::cout << "Increment is called." << std::endl;
                return Value(arg["data"].intValue() + 1);
            });
    }

}