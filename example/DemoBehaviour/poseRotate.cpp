#include <iostream>

#include <juiz/juiz.h>
#include <juiz/operation.h>

using namespace juiz;

extern "C" {

    JUIZ_OPERATION void* poseRotate() {
        return operationFactory({ {"typeName", "poseRotate"},
              {"defaultArg", {
                {"arg01", 0},
                {"rotateAngle", 0},
                {"pose", {
                    {"position", {
                        {"x", 0.0},
                        {"y", 0.0},
                        {"z", 0.0}
                    }},
                    {"orientation", {
                        {"x", 0.0},
                        {"y", 0.0},
                        {"z", 0.0},
                        {"w", 1.0}
                    }}
                }}
              }}
            },
            [](auto arg) {
              logger::warn("poseRotate: ", arg);
                return arg["pose"];
            });
    }
}