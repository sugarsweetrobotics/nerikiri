#pragma once

#include "nerikiri/value.h"

namespace nerikiri {



    class Object {
    protected:
      Value info_;
    public:
      Value info() const { return info_; };
      Value setInstanceName(const std::string& name) {
          info_["instanceName"] = name;
          return info_;
      }

    public:
      Object(const Value& info) : info_(info) {}
      virtual ~Object() {}

    };
}