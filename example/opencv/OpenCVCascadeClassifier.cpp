//#include "nerikiri/nerikiri.h"
#include "nerikiri/containers/containerfactory.h"
#include "OpenCVCascadeClassifier.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* createOpenCVCascadeClassifier() {
        return containerFactory<OpenCVCascadeClassifier>();
    }
}
