#pragma once

#include "state.h"
#include <string>
#include <map>

namespace nerikiri {

  class StateMap {
  private:
    std::map<std::string, nerikiri::State> map_;
  public:
    StateMap();
    ~StateMap();

  public:
    State& operator[](const std::string& x) {
      return map_[x];
    }

    State& operator[](const char* x) {
      return map_[x];
    }

  };


}
