#pragma once

#include <juiz/nerikiri.h>
#include <juiz/value.h>
#include <juiz/utils/naming.h>
#include <juiz/logger.h>

namespace juiz {

  template<class Cls> 
  inline static std::string nullClsNameString() { return std::string("Null") + Cls::classNameString; }
  template<class Cls> 
  inline const std::string factoryClsNameString() { return Cls::classNameString + std::string("Factory"); }
  template<class Cls> 
  inline const std::string nullFactoryClsNameString() { return std::string("Null") +  Cls::classNameString + "Factory"; }


  /**
   */
  class Object {
  public:
    static constexpr char classNameString[] = "Object"; 
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

    /**
     * create null object
     */
    Object();

    virtual ~Object();

  public:
    virtual void finalize() {}

    virtual const std::string& description() const { return description_; }
    virtual void setDescription(const std::string& description) { description_ = description; }
    
    virtual ClassName className() const { 
      return className_;
    }

    virtual void setClassName(const std::string& className) { className_ = className; }

    virtual TypeName typeName() const { 
      return typeName_;
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

  //static char* const Object_className = "Object";

    //static char const Object_className[7] = "Object";
  template<typename T>
  std::shared_ptr<T> nullObject();

  inline std::shared_ptr<Object> createNullObject() { return std::make_shared<Object>(); }

  template<>
  inline std::shared_ptr<Object> nullObject() { return createNullObject(); }
}