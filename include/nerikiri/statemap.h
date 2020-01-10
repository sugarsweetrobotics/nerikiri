#pragma once

#include "state.h"

namespace nerikiri {

  class StateMap {

  public:
    StateMap();
    ~StateMap();

  public:
    State& operator[](const std::string& x);
    State& operator[](std::string&& x);
    State& operator[](const char* x);
  };


}
