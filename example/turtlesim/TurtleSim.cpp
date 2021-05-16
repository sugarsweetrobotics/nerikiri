#include <juiz/juiz.h>
#include <juiz/container.h>
#include "TurtleSim.h"

using namespace juiz;

extern "C" {

    NK_OPERATION  void* createTurtleSim() {
        return containerFactory<TurtleSim>();
    }

}