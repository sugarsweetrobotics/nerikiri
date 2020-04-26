//#include "nerikiri/nerikiri.h"
#include "nerikiri/containerfactory.h"
#include "JoyController.h"

using namespace nerikiri;

extern "C" {
    
    void* createJoyController() {
        return containerFactory<JoyController>();
    }

};

