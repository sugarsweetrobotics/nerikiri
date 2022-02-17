#include <iostream>

#include <juiz/juiz.h>
#include <juiz/operation.h>

using namespace juiz;

extern "C" {

    JUIZ_OPERATION void* operationName() {
        return operationFactory(
          {
            {"typeName", "operationName"},
            {"defaultArg", {
                {}
            }},
          },
          [](auto arg) {
              
              return Value::error("operationName is not implemented yet.");
          });
    }
}
