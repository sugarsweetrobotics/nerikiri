#pragma once

#include "nerikiri/nerikiri.h"
#include "nerikiri/value.h"

namespace nerikiri {



    class Object {
    protected:
      Value info_;
      bool is_null_;
    public:
      Value info() const { return info_; };
      Value setInstanceName(const std::string& name) {
          info_["instanceName"] = name;
          return info_;
      }

      Value setState(const std::string& state) { 
        info_["state"] = state;
        return info_;
      }

      std::string getState() const { 
        return info_.at("state").stringValue();
      }

      bool isNull() const { return is_null_; }

      std::string getInstanceName() const { 
        if (info_.objectValue().count("instanceName") == 0) { return ""; }
        else if (!info_.at("instanceName").isStringValue()) { 
          return ""; 
        }
        return info_.at("instanceName").stringValue();
      }

    public:
      Object(): info_({{"name", "null"}, {"state", "created"}}), is_null_(true) {}
      Object(const Value& info) : info_(info), is_null_(false) {
        info_["state"] = "created";
      }
      virtual ~Object() {}

    };
}