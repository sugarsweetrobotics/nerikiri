#include <iostream>

#include <juiz/value.h>

#include <juiz/logger.h>
#include <juiz/utils/functional.h>
#include "operation_base.h"
#include <juiz/operation.h>

using namespace juiz;

namespace juiz {

    std::shared_ptr<OperationAPI> createOperation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}, const std::function<Value(const Value&)>& func = nullptr, const Value& info={}); 

    std::shared_ptr<OperationAPI> containerOperationBase(const std::string& _typeName, const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {});

}

class Operation : public OperationBase {
private:
  std::mutex op_mutex_;
  std::function<Value(const Value&)> function_;
  
public:
  Operation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}, const std::function<Value(const Value&)>& func = nullptr, const Value& info={}): OperationBase("Operation", operationTypeName, _fullName, defaultArgs), function_((func)) {
    if (info.isObjectValue()) {
      if (info.hasKey("description") && info.at("description").isStringValue()) {
        setDescription(info["description"].stringValue());
      }
    }
  }

  virtual ~Operation() {}

  virtual Value call(const Value& value) override  {
    logger::trace("Operation({})::call({})", info().at("fullName"), value);
    std::lock_guard<std::mutex> lock(op_mutex_);
    return this->function_(value); 
  }
};


std::shared_ptr<OperationAPI> juiz::createOperation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs, const std::function<Value(const Value&)>& func, const Value& info) {
  Value args = defaultArgs;
  if (info.hasKey("defaultArgs")) {
    args = Value::merge(defaultArgs, info["defaultArgs"]);
  }
  return std::make_shared<Operation>(operationTypeName, _fullName, args, func, info);
}