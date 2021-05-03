//#include "nerikiri/nerikiri.h"
#include <nerikiri/container.h>
#include "OpenCVCamera.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* createOpenCVCamera() {
        return containerFactory<OpenCVCamera>();
    }
}
