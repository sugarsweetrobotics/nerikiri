#include "nerikiri/container_factory.h"
#include "KeyboardJoy.h"

using namespace nerikiri;

extern "C" {
    

    NK_OPERATION  void* createKeyboardJoy() {
        return containerFactory<KeyboardJoy>();
    }

};

