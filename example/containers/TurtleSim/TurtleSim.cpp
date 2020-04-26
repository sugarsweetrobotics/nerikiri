//#include "nerikiri/nerikiri.h"
#include "nerikiri/containerfactory.h"
#include "TurtleSim.h"

using namespace nerikiri;

extern "C" {
    void* createTurtleSim();
};


void* createTurtleSim() {
    return containerFactory<TurtleSim>();
}
