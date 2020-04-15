#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/operationfactory.h"

using namespace nerikiri;

extern "C" {
    void* add();
};


void* add() {
    return operationFactory({{"name", "add"},
          {"defaultArg", {
            {"arg01", 0},
            {"arg02", 0}
          }}
        },
		   [](auto arg) { 
		     return Value({{"result", arg["arg01"].intValue() + arg["arg02"].intValue()}});
		   });
}