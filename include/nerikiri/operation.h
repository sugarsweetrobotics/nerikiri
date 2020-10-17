#pragma once

#include <functional>
#include <mutex>
#include <thread>
//#include "nerikiri/logger.h"
#include "nerikiri/value.h"
#include "nerikiri/operationbase.h"

namespace nerikiri {

  class Operation : public OperationBase {
  private:
    std::mutex mutex_;
    std::function<Value(const Value&)> function_;
    
  public:
    Operation(): OperationBase(), function_(nullptr) {
    }

    Operation(Value&& info) : OperationBase(std::move(info)), function_(nullptr) {
    }

    Operation(Value&& info, std::function<Value(const Value&)> func): OperationBase(std::move(info)), function_((func)) {}

    Operation(const Value& info, std::function<Value(const Value&)> func) : OperationBase(info), function_(func) {
      
    }

    virtual ~Operation() {}

  public:
    Operation& operator=(const Operation& op) {
      operator=(op);
      this->function_ = op.function_;
      return *this;
    }

    virtual Value call(const Value& value) override;
  };
}
