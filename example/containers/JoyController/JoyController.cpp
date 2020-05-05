//#include "nerikiri/nerikiri.h"
#include "nerikiri/containers/containerfactory.h"
#include "JoyController.h"

using namespace nerikiri;

extern "C" {
    

    NK_OPERATION  void* createJoyController() {
        return containerFactory<JoyController>();
    }

};

