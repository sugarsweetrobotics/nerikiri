#include <juiz/juiz.h>
#include <juiz/container.h>

#include "DemoBehaviour.h"

using namespace juiz;


extern "C" {

    JUIZ_OPERATION  void* DemoBehaviour_start() {
        return containerOperationFactory<DemoBehaviour>(
            {
              {"typeName", "start"},
              {"defaultArg", {
                  {}
              }},
            },
            [](auto& container, auto arg) {
                container.count = 0;
                return Value::error("DemoBehaviour_start is not implemented yet.");
            });
    }

}
