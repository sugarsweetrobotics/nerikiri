#pragma once

#include "nerikiri/value.h"
#include <string>
#include <map>

namespace nerikiri {

  class ValueMap {
  private:
    std::map<std::string, nerikiri::Value> map_;
  public:
    ValueMap();
    ~ValueMap();

  public:
    Value& operator[](const std::string& x) {
      return map_[x];
    }

    Value& operator[](const char* x) {
      return map_[x];
    }

  };


}
