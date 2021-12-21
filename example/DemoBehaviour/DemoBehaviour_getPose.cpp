#include <cmath>

#include <juiz/juiz.h>
#include <juiz/container.h>
#include <juiz/geometry.h>
#include "DemoBehaviour.h"


using namespace juiz;


extern "C" {

    JUIZ_OPERATION  void* DemoBehaviour_getPose() {
        return containerOperationFactory<DemoBehaviour>(
            {
              {"typeName", "getPose"},
              {"defaultArg", {
                  {}
              }},
            },
            [](auto& container, auto arg) {
                return toValue(container.pose);
            }
        );
    }

}
