#include <iostream>

#include <juiz/nerikiri.h>
#include <juiz/operation.h>

using namespace juiz;

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