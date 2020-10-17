#pragma once

#include "nerikiri/nerikiri.h"
#include "nerikiri/value.h"

#include "nerikiri/logger.h"

namespace nerikiri {


  inline std::pair<std::string, std::string> separateNamespaceAndInstanceName(const std::string& fullName) {
    auto tokens = nerikiri::stringSplit(fullName, ':');
    return {stringJoin(tokens.begin(), tokens.end()-1, ':'), tokens[tokens.size()-1]};
  }

  /**
   */
  class Object {
  private:
    
  public:

    Object(const Value& info) : info_(info){
      info_["state"] = "created";
      if (!info_.hasKey("typeName")) {
        nerikiri::logger::error("Object::Object(const Value&): Warning. No typeName member is included in the argument 'info'");
      } else if (!info_.hasKey("instanceName")) {
        nerikiri::logger::error("Object::Object(const Value&): Warning. No instanceName member is included in the argument 'info'");
      } else if (!info_.hasKey("fullName")) {
        nerikiri::logger::error("Object::Object(const Value&): Warning. No fullName member is included in the argument 'info'");
      }
    }

    Object(): Object({{"typeName", "null"}, {"instanceName", "null"}, {"fullName", "null"}, {"state", "created"}}) {}


    virtual ~Object() {}

  public:

    TypeName typeName() const { 
      return info_.at("typeName").stringValue();
    }



    virtual bool isInstanceOf(const TypeName& _typeName) const { 
      return(typeName() == _typeName);
    }
    
    bool isNull() const { return typeName() == "null"; }


    std::string fullName() const { 
      if (info_.objectValue().count("fullName") == 0) { return ""; }
      else if (!info_.at("fullName").isStringValue()) { 
        return ""; 
      }
      return info_.at("fullName").stringValue();
    }

    std::string getFullName() const { return fullName(); }

    virtual Value setFullName(const std::string& nameSpace, const std::string& name) {
      info_["instanceName"] = name;
      if (nameSpace.length() == 0) info_["fullName"] = name;
      else info_["fullName"] = nameSpace + ":" + name;
      return info_;
    }

    virtual Value setFullName(const std::string& fullName) {
      auto [nameSpace, instanceName] = separateNamespaceAndInstanceName(fullName);
      info_["instanceName"] = instanceName;
      info_["fullName"] = fullName;
      return info_;
    }  
    
    std::string instanceName() const { 
      return info_.at("instanceName").stringValue();
    }

    std::string getInstanceName() const { return instanceName();
    }

  protected:
    Value info_;

  protected: 
    void _setNull() { info_["typeName"] = "null"; }

  public:
    Value info() const { return info_; };



    Value setState(const std::string& state) { 
      info_["state"] = state;
      return info_;
    }

    std::string getState() const { return state();
    }

    std::string state() const { 
      return info_.at("state").stringValue();
    }



  };
}