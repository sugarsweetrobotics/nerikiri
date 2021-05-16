#include <iostream>

#include <juiz/nerikiri.h>
#include <juiz/operation.h>

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