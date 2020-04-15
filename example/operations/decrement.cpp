#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/operationfactory.h"

using namespace nerikiri;

extern "C" {
    void* decrement();
};


void* decrement() {
    return operationFactory( {{"name", "decrement"},
          {"defaultArg", {
            {"arg01", 0}
          }}
        },
		   [](auto arg) { 
		     return Value(arg["arg01"].intValue() - 1);
		   });
}