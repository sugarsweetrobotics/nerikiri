#include <juiz/container.h>
#include "KeyboardJoy.h"

using namespace juiz;

extern "C" {
    

    NK_OPERATION  void* createKeyboardJoy() {
        return containerFactory<KeyboardJoy>();
    }

};

