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

      std::string getInstanceName() const { 
        if (info_.objectValue().count("instanceName") == 0) { return ""; }
        else if (!info_.at("instanceName").isStringValue()) { 
          return ""; 
        }
        return info_.at("instanceName").stringValue();
      }

    public:
      Object(const Value& info) : info_(info) {}
      virtual ~Object() {}

    };
}