#include <iostream>

#include <juiz/juiz.h>
#include <juiz/operation.h>

using namespace juiz;

extern "C" {

    JUIZ_OPERATION  void* one() {
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

