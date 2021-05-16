#include <juiz/nerikiri.h>
#include <juiz/container.h>
#include "TurtleSim.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* createTurtleSim() {
        return containerFactory<TurtleSim>();
    }

}