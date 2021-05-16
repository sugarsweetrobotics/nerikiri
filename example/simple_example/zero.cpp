#include <iostream>

#include <juiz/juiz.h>
#include <juiz/operation.h>

using namespace juiz;

extern "C" {

	JUIZ_OPERATION  void* zero() {
		return operationFactory({ {"typeName", "zero"},
			  {"defaultArg", {}}
			},
			[](auto arg) {
				return Value({ 0 });
			});
	}
}