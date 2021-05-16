
#include <juiz/container.h>
#include "JoyController.h"

using namespace juiz;

extern "C" {
    

    NK_OPERATION  void* createJoyController() {
        return containerFactory<JoyController>();
    }

};

