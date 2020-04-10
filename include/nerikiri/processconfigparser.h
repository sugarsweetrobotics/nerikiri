#pragma once

#include <cstdio>

#include "nerikiri/value.h"
#include "nerikiri/datatype/json.h"


namespace nerikiri {



    class ProcessConfigParser {
    public:


    public:
      ProcessConfigParser();

    public:
      static Value parseConfig(std::FILE* fp) {
          return nerikiri::json::toValue(fp);
      }
    };
}