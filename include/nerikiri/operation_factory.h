#pragma once

#include <nerikiri/operation_factory_api.h>


namespace nerikiri {


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

  inline void* operationFactory(const Value& info, std::function<Value(const Value&)>&& func) { return new OperationFactory(Value::string(info.at("typeName")), info.at("defaultArgs"), func); }
}