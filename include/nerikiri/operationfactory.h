#pragma once


#include "nerikiri/operation.h"
#include "nerikiri/value.h"

namespace nerikiri {

  class OperationFactory {
  private:
    Value info_;
    std::function<Value(const Value&)> function_;
  public:
    std::string typeName() { return info_.at("name").stringValue(); }
  public:
    OperationFactory(const Value& info, std::function<Value(const Value&)>&& func): info_(info), function_(func) {}
    virtual ~OperationFactory() {}

    std::shared_ptr<Operation> create(const Value& info) {
      auto i = nerikiri::merge(info, info_);
        return std::make_shared<Operation>(i, function_); 
    }
  };

  inline void* operationFactory(const Value& info, std::function<Value(const Value&)>&& func) { return new OperationFactory(info, std::move(func)); }
}