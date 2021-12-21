#include <juiz/juiz.h>
#include <juiz/container.h>

#include "DemoBehaviour.h"

using namespace juiz;

extern "C" {

    JUIZ_OPERATION  void* createDemoBehaviour() {
        return containerFactory<DemoBehaviour>();
    }

}

