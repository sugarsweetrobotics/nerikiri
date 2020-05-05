//#include "nerikiri/nerikiri.h"
#include "nerikiri/containers/containerfactory.h"
#include "MyStruct.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* createMyStruct() {
        return containerFactory<MyStruct>();
    }
}
