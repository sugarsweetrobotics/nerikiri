#include <iostream>

#include <juiz/juiz.h>
#include <juiz/operation.h>

using namespace juiz;

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