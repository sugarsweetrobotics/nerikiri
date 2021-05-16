#include <iostream>

#include <juiz/nerikiri.h>
#include <juiz/operation.h>

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

