//#include "nerikiri/nerikiri.h"
#include "nerikiri/container_factory.h"
#include "JoyController.h"

using namespace nerikiri;

extern "C" {
    

    NK_OPERATION  void* createJoyController() {
        return containerFactory<JoyController>();
    }

};

