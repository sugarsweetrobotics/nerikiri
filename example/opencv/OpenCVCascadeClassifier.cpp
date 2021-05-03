//#include "nerikiri/nerikiri.h"
#include <nerikiri/container.h>
#include "OpenCVCascadeClassifier.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* createOpenCVCascadeClassifier() {
        return containerFactory<OpenCVCascadeClassifier>();
    }
}
