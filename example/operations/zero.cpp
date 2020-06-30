#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/operationfactory.h"

using namespace nerikiri;

extern "C" {

	NK_OPERATION  void* zero() {
		return operationFactory({ {"typeName", "zero"},
			  {"defaultArg", {}}
			},
			[](auto arg) {
				return Value({ 0 });
			});
	}
}