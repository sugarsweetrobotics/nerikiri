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
    ClassName className_;
    TypeName  typeName_;
    std::string fullName_;
    std::string instanceName_;
    std::string description_;
  public:
    Object(const std::string& typeName, const std::string& fullName);

    Object(const std::string& className, const std::string& typeName, const std::string& fullName);

    Object(const Value& info);

    Object();

    virtual ~Object();

  public:
    virtual void finalize() {}

    virtual const std::string& description() const { return description_; }
    virtual void setDescription(const std::string& description) { description_ = description; }
    
    virtual ClassName className() const { 
      return className_;
      //return info_.at("className").stringValue();
    }

    virtual TypeName typeName() const { 
      return typeName_;
      //return info_.at("typeName").stringValue();
    }

    virtual void setTypeName(const std::string& typeName) { typeName_ = typeName; }

    virtual bool isInstanceOf(const TypeName& _typeName) const { 
      return(typeName() == _typeName);
    }
    
    bool isNull() const { return fullName() == "null"; }

    virtual std::string fullName() const;

    std::string getFullName() const;

    virtual Value setFullName(const std::string& nameSpace, const std::string& name);

    virtual Value setFullName(const std::string& fullName);
    
    virtual std::string instanceName() const;

    std::string getInstanceName() const;

  protected:
    Value info_;

  protected: 
    void _setNull() { info_["typeName"] = "null"; }

  public:

    virtual Value info() const;

    virtual Value fullInfo() const { return info(); }

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