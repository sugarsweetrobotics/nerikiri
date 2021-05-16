#include <juiz/container.h>
#include "KeyboardJoy.h"

using namespace juiz;

extern "C" {
    

    JUIZ_OPERATION  void* createKeyboardJoy() {
        return containerFactory<KeyboardJoy>();
    }

};

