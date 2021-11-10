#include <juiz/juiz.h>
#include <juiz/container.h>

#include "ContainerName.h"

using namespace juiz;


extern "C" {

    JUIZ_OPERATION  void* ContainerName_operationName() {
        return containerOperationFactory<ContainerName>(
            {
              {"typeName", "operationName"},
              {"defaultArg", {
                  {}
              }},
            },
            [](auto& container, auto arg) {
                
                return Value::error("ContainerName_operationName is not implemented yet.");
            }
        );
    }

}