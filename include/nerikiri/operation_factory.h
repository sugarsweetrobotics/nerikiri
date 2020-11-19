#pragma once

#include <nerikiri/operation_factory_api.h>


namespace nerikiri {


  class OperationFactory : public OperationFactoryAPI{
  private:
    const Value defaultArgs_;
    const std::string operationTypeFullName_;
    const std::function<Value(const Value&)> function_;
  public:
    OperationFactory(const Value& info, const std::function<Value(const Value&)>& func) : OperationFactory(Value::string(info.at("typeName")), info.at("defaultArg"), func){}
    OperationFactory(const std::string& operationTypeFullName, const Value& defaultArgs, const std::function<Value(const Value&)>& func):
      OperationFactoryAPI("OperationFactory", operationTypeFullName, operationTypeFullName + "0"), defaultArgs_(defaultArgs), operationTypeFullName_(operationTypeFullName), function_(func) {}
    virtual ~OperationFactory() {}

    virtual std::shared_ptr<OperationAPI> create(const std::string& _fullName) const {
      return createOperation(typeName(), _fullName, defaultArgs_, function_); 
    }
  };

  inline void* operationFactory(const Value& info, std::function<Value(const Value&)>&& func) { return new OperationFactory(Value::string(info.at("typeName")), info.at("defaultArg"), func); }
}