#include <iostream>

#include <juiz/value.h>

#include <juiz/logger.h>
#include <juiz/utils/functional.h>
#include "operation_base.h"
#include <juiz/operation.h>

using namespace juiz;

namespace juiz {

    std::shared_ptr<OperationAPI> createOperation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}, const std::function<Value(const Value&)>& func = nullptr); 

    std::shared_ptr<OperationAPI> containerOperationBase(const std::string& _typeName, const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {});

}

class Operation : public OperationBase {
private:
  std::mutex mutex_;
  std::function<Value(const Value&)> function_;
  
public:
  Operation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}, const std::function<Value(const Value&)>& func = nullptr): OperationBase("Operation", operationTypeName, _fullName, defaultArgs), function_((func)) {}

  virtual ~Operation() {}

  virtual Value call(const Value& value) override  {
    logger::trace("Operation({})::call({})", info().at("fullName"), value);
    std::lock_guard<std::mutex> lock(mutex_);
    return this->function_(value); 
  }
};


std::shared_ptr<OperationAPI> juiz::createOperation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs, const std::function<Value(const Value&)>& func) {
    return std::make_shared<Operation>(operationTypeName, _fullName, defaultArgs, func);
}