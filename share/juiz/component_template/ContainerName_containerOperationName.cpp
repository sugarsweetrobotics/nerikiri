#include <juiz/juiz.h>
#include <juiz/container.h>

#include "ContainerName.h"

using namespace juiz;


extern "C" {

    JUIZ_OPERATION  void* ContainerName_containerOperationName() {
        return containerOperationFactory<ContainerName>(
            {
              {"typeName", "containerOperationName"},
              {"defaultArg", {
                  {}
              }},
            },
            [](auto& container, auto arg) {
                
                return Value::error("ContainerName_containerOperationName is not implemented yet.");
            }
        );
    }

}