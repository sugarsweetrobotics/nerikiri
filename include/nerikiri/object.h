#pragma once

#include <nerikiri/nerikiri.h>
#include <nerikiri/value.h>
#include <nerikiri/naming.h>
#include <nerikiri/logger.h>

namespace nerikiri {


  /**
   */
  class Object {
  private:
    
  public:
    Object(const std::string& typeName, const std::string& fullName) {
      auto [nameSpace, instanceName] = separateNamespaceAndInstanceName(fullName);
      info_ = Value{
        {"typeName", typeName},
        {"instanceName", instanceName},
        {"fullName", fullName}
      };
    }

    Object(const Value& info) : info_(info){
      info_["state"] = "created";
      if (!info_.hasKey("typeName")) {
        nerikiri::logger::error("Object::Object(const Value& v={}): Error. No typeName member is included in the argument 'info'", info);
      } else if (!info_.hasKey("instanceName")) {
        if (info_.hasKey("fullName")) {
          nerikiri::logger::warn("Object::Object(const Value& v={}): Warning. No instanceName member is included in the argument 'info', but info has fullName member, so the instanceName of this object is now '{}'", info, info.at("fullName"));
          info_["instanceName"] = info_["fullName"];
        } else {
          nerikiri::logger::error("Object::Object(const Value& v={}): Error. No instanceName member is included in the argument 'info'", info);
        }
      } else if (!info_.hasKey("fullName")) {
        if (info_.hasKey("instanceName")) {
          nerikiri::logger::warn("Object::Object(const Value& v={}): Warning. No fullName member is included in the argument 'info', but info has instanceName member, so the fullName of this object is now '{}'", info, info.at("instanceName"));
          info_["fullName"] = info_["instanceName"];
        } else {
          nerikiri::logger::error("Object::Object(const Value& v={}): Error. No fullName member is included in the argument 'info'", info);
        }
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

    virtual Value info() const { return {
        {"fullName", fullName()},
        {"instanceName", instanceName()},
        {"typeName", typeName()},
        {"state", objectState()}
      }; 
    }



    Value setObjectState(const std::string& state) { 
      info_["state"] = state;
      return info_;
    }

    std::string getObjectState() const { return objectState();
    }

    std::string objectState() const { 
      return info_.at("state").stringValue();
    }

    /**
     * Release all resources
     */
    virtual void invalidate() {}

  };
}