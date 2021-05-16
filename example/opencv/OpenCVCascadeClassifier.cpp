
#include <juiz/container.h>
#include "OpenCVCascadeClassifier.h"

using namespace juiz;

extern "C" {

    NK_OPERATION  void* createOpenCVCascadeClassifier() {
        return containerFactory<OpenCVCascadeClassifier>();
    }
}
