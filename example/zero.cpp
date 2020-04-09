#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/operationfactory.h"

using namespace nerikiri;

extern "C" {
    void* zero();
};


void* zero() {
    return operationFactory( {{"name", "zero"},
          {"defaultArg", {}}
        },
		   [](auto arg) { 
		     return Value({0});
		   });
}
