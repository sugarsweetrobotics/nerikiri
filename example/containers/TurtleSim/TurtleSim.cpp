#include "nerikiri/nerikiri.h"
#include "nerikiri/containers/containerfactory.h"
#include "TurtleSim.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* createTurtleSim() {
        return containerFactory<TurtleSim>();
    }

}