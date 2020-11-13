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
    Object(const std::string& typeName, const std::string& fullName);

    Object(const std::string& className, const std::string& typeName, const std::string& fullName);

    Object(const Value& info);

    Object();

    virtual ~Object();

  public:

    ClassName className() const { 
      return info_.at("className").stringValue();
    }

    TypeName typeName() const { 
      return info_.at("typeName").stringValue();
    }

    virtual bool isInstanceOf(const TypeName& _typeName) const { 
      return(typeName() == _typeName);
    }
    
    bool isNull() const { return fullName() == "null"; }


    std::string fullName() const;

    std::string getFullName() const;

    virtual Value setFullName(const std::string& nameSpace, const std::string& name);

    virtual Value setFullName(const std::string& fullName);
    
    std::string instanceName() const;

    std::string getInstanceName() const;

  protected:
    Value info_;

  protected: 
    void _setNull() { info_["typeName"] = "null"; }

  public:

    virtual Value info() const;

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