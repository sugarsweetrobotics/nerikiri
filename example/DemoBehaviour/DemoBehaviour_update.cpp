#include <iostream>

#include <juiz/juiz.h>
#include <juiz/container.h>

#include "DemoBehaviour.h"

using namespace juiz;


extern "C" {

    JUIZ_OPERATION  void* DemoBehaviour_update() {
        return containerOperationFactory<DemoBehaviour>(
            {
              {"typeName", "update"},
              {"defaultArg", {
                  {}
              }},
            },
            [](auto& container, auto arg) {
                container.count++;
                double x = 1.0 * sin(container.count / 50.0);
                double y = 1.0 * cos(container.count / 50.0);
                double z = 0;
                container.pose = Pose3D({x, y, z}, {0, 0, 0, 1.0});
                std::cout << "DemoBehaviour_update(): " << toString(container.pose) << std::endl;
                return toValue(container.pose);
            }
        );
    }

}
