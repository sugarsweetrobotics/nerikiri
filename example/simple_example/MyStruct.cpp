//#include "juiz/juiz.h"
#include <juiz/container.h>
#include "MyStruct.h"

using namespace juiz;

extern "C" {

    JUIZ_OPERATION  void* createMyStruct() {
        return containerFactory<MyStruct>();
    }
}
