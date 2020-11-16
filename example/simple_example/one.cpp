#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/operation_factory.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* one() {
        return operationFactory({
            {"typeName", "one"},
            {"defaultArg", {}}
            }, [](auto arg) {
                static int i;
                std::cout << "One is called (count=" << i++ << ")" << std::endl;
                return Value({ 1 });
            });
    }
}
