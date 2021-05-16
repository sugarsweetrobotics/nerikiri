
#include <juiz/container.h>
#include "OpenCVCamera.h"

using namespace juiz;

extern "C" {

    NK_OPERATION  void* createOpenCVCamera() {
        return containerFactory<OpenCVCamera>();
    }
}
