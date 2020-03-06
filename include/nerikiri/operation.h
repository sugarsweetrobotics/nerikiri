#pragma once

#include <map>
#include <functional>

#include "nerikiri/valuemap.h"
#include "nerikiri/operationinfo.h"
#include "nerikiri/connection.h"

namespace nerikiri {
  class Operation;
  class Process;
  using Process_ptr = Process*;
  
  Value call_operation(const Operation& operation, Value&& value);
 
  class Operation {
  private:
    Process_ptr process_;
    std::function<Value(Value)> function_;
    OperationInfo info_;
    bool is_null_;
    ConnectionList providerConnectionList_;
    ConnectionListDictionary consumerConnectionListDictionary_;
  public:
    static Operation null;
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

    friend Value call_operation(const Operation& operation, Value&& value);

    Value addProviderConnection(Connection&& c) {
      return c.info();
    }

    Value addConsumerConnection(Connection&& c) {
      return c.info();
    }

    ConnectionList getProviderConnectionList() const {
      return providerConnectionList_;
    }
    ConnectionList getConsumerConnectionsByArgName(const std::string& argName) const {
      return consumerConnectionListDictionary_.at(argName);
    }

    bool hasConsumerConnection(const ConnectionInfo& ci) const {
      for(const auto v : getConsumerConnectionsByArgName(ci.at("to").at("name").stringValue())) {
        if (v.info().at("from") == ci.at("from")) return true;
      }
      return false;
    }

    bool hasProviderConnection(const ConnectionInfo& ci) const {
      for(const auto v : providerConnectionList_) {
        if (v.info().at("to") == ci.at("to")) return true;
      }
      return false;
    }
  };
  
}
