#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/operationfactory.h"

using namespace nerikiri;

extern "C" {
    void* increment();
};


void* increment() {
    return operationFactory( 
        {
            {"name", "increment"},
            {"defaultArg", {
                {"arg01", 0}
            }}
        },
		[](auto arg) { 
          std::cout << "Increment is called." << std::endl;
		    return Value(arg["arg01"].intValue() + 1);
		});
}
