#include <juiz/juiz.h>
#include <juiz/container.h>

#include "ContainerName.h"

using namespace juiz;

extern "C" {

    JUIZ_OPERATION  void* createContainerName() {
        return containerFactory<ContainerName>();
    }

}

