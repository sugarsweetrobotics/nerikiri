#include "nerikiri/nerikiri.h"
#include "nerikiri/containerfactory.h"

#include "MyStruct.h"

using namespace nerikiri;

extern "C" {
    void* createMyStruct();
};


void* createMyStruct() {
    return containerFactory<MyStruct>();
}
