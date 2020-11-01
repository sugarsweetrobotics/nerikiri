#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include "nerikiri/operation_base.h"

namespace nerikiri {

  class Operation : public OperationBase {
  private:
    std::mutex mutex_;
    std::function<Value(const Value&)> function_;
    
  public:
    Operation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}, const std::function<Value(const Value&)>& func = nullptr): OperationBase("Operation", operationTypeName, _fullName, defaultArgs), function_((func)) {}

    virtual ~Operation() {}

    virtual Value call(const Value& value) override;
  };
}
