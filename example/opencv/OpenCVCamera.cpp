//#include "nerikiri/nerikiri.h"
#include "nerikiri/containers/containerfactory.h"
#include "OpenCVCamera.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* createOpenCVCamera() {
        return containerFactory<OpenCVCamera>();
    }
}
