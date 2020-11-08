#pragma once

#include <nerikiri/operation_api.h>
#include <nerikiri/operation.h>


namespace nerikiri {

  class OperationFactoryAPI : public Object {
  public:
    virtual std::string operationTypeFullName() const = 0;
  public:
    OperationFactoryAPI(const std::string& typeName, const std::string& fullName): Object(typeName, fullName) {}
    virtual ~OperationFactoryAPI() {}

    virtual std::shared_ptr<OperationAPI> create(const std::string& _fullName) const = 0;
  };

  class NullOperationFactory : public OperationFactoryAPI {
  public:
    virtual std::string operationTypeFullName() const override { return "NullOperation"; }
  public:
    NullOperationFactory() : OperationFactoryAPI("NullOperationFactory", "null") {}
    virtual ~NullOperationFactory() {}

    virtual std::shared_ptr<OperationAPI> create(const std::string& _fullName) const override {
      return std::make_shared<NullOperation>();
    }
  };

  /*
  class OperationFactory : public OperationFactoryAPI{
  private:
    const Value defaultArgs_;
    const std::string operationTypeFullName_;
    const std::function<Value(const Value&)> function_;
  public:
    virtual std::string operationTypeFullName() const { return operationTypeFullName_;; }
  public:
    OperationFactory(const std::string& operationTypeFullName, const Value& defaultArgs, const std::function<Value(const Value&)>& func):
      OperationFactoryAPI("OperationFactory", operationTypeFullName + "Factory"), defaultArgs_(defaultArgs), operationTypeFullName_(operationTypeFullName), function_(func) {}
    virtual ~OperationFactory() {}

    virtual std::shared_ptr<OperationAPI> create(const std::string& _fullName) const {
      return std::make_shared<Operation>(operationTypeFullName(), _fullName, defaultArgs_, function_); 
    }
  };
  */

  //inline void* operationFactory(const Value& info, std::function<Value(const Value&)>&& func) { return new OperationFactory(info, func); }
}