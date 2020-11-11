#include "nerikiri/nerikiri.h"
#include "nerikiri/container_factory.h"
#include "TurtleSim.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* createTurtleSim() {
        return containerFactory<TurtleSim>();
    }

}