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
                {"data", 0}
            }}
        },
		[](auto arg) { 
          std::cout << "Increment is called." << std::endl;
		    return Value(arg["data"].intValue() + 1);
		});
}
