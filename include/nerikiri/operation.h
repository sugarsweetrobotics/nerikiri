#pragma once


#include <functional>

#include "statemap.h"

namespace nerikiri {

  class Operation {

  public:
    Operation(std::function<StateMap(StateMap)>&& func);
    ~Operation();

  public:
    

  };


}
