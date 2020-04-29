#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/operationfactory.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* one() {
        return operationFactory({
            {"name", "one"},
            {"defaultArg", {}}
            }, [](auto arg) {
                static int i;
                std::cout << "One is called (count=" << i++ << ")" << std::endl;
                return Value({ 1 });
            });
    }
}

