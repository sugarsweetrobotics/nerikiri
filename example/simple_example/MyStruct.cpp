//#include "nerikiri/nerikiri.h"
#include <juiz/container.h>
#include "MyStruct.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* createMyStruct() {
        return containerFactory<MyStruct>();
    }
}
