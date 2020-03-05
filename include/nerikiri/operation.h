#pragma once

#include <map>
#include <functional>

#include "nerikiri/valuemap.h"
#include "nerikiri/operationinfo.h"
#include "nerikiri/connection.h"

namespace nerikiri {
  class Operation;

  Value call_operation(const Operation& operation, Value&& value);
 
  class Operation {
  private:
    std::function<Value(Value)> function_;
    OperationInfo info_;
    bool is_null_;
    ConnectionListDictionary connectionListDictionary_;
  private:
    
  public:
    Operation(): is_null_(true) {}
    Operation(OperationInfo&& info, std::function<Value(Value)>&& func);
    Operation(const OperationInfo& info, std::function<Value(Value)>&& func);
    Operation(const Operation& op): function_(op.function_), info_(op.info_), is_null_(op.is_null_){}

    Operation& operator=(const Operation& op) {
      function_ = op.function_;
      info_ = op.info_;
      is_null_ = op.is_null_;
      return *this;
    }
    ~Operation();

  public:
    const OperationInfo& info() const { return info_; }
    bool isNull() const { return is_null_; }

    static Operation null;

    friend Value call_operation(const Operation& operation, Value&& value);

    ConnectionList getConnectionsByArgName(const std::string& argName) {
      return connectionListDictionary_[argName];
    }
  };
  
}
