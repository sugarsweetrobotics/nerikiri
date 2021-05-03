//#include "nerikiri/nerikiri.h"
#include "nerikiri/container.h"
#include "MyStruct.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* createMyStruct() {
        return containerFactory<MyStruct>();
    }
}
