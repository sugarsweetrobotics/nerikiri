
#include <juiz/container.h>
#include "OpenCVCamera.h"

using namespace juiz;

extern "C" {

    JUIZ_OPERATION  void* createOpenCVCamera() {
        return containerFactory<OpenCVCamera>();
    }
}
