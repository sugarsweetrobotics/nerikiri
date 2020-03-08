#pragma once

#include <map>
#include <functional>
#include "nerikiri/logger.h"
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
    Operation(const Operation& op): process_(op.process_), function_(op.function_), info_(op.info_), is_null_(op.is_null_),
      providerConnectionList_(op.providerConnectionList_), consumerConnectionListDictionary_(op.consumerConnectionListDictionary_)
    {
      
    }

    Operation& operator=(const Operation& op) {
      process_ = op.process_;
      function_ = op.function_;
      info_ = op.info_;
      is_null_ = op.is_null_;
      providerConnectionList_ = op.providerConnectionList_;
      consumerConnectionListDictionary_ = op.consumerConnectionListDictionary_;
      return *this;
    }
    ~Operation();

  public:
    const OperationInfo& info() const { return info_; }
    bool isNull() const { return is_null_; }

    friend Value call_operation(const Operation& operation, Value&& value);

    Value addProviderConnection(Connection&& c) {
      logger::trace("Operation::addProviderConnection({})", str(c.info()));
      providerConnectionList_.emplace_back(std::move(c));
      return c.info();
    }

    Value removeProviderConnection(const ConnectionInfo& ci) {
      // 未実装
      return ci;
    }

    Value addConsumerConnection(Connection&& c) {
      logger::trace("Operation::addConsumerConnection({})", str(c.info()));
      
      if (c.info()["consumer"]["name"] != info().at("name")) {
          logger::error("requestted connection does not match to this operation.");
      }

      const auto argumentName = c.info()["consumer"]["target"]["name"].stringValue();
      if (info().at("defaultArg").hasKey(argumentName)) {
        auto inf = c.info();
        consumerConnectionListDictionary_[argumentName].emplace_back(std::move(c));
        return inf;
      }
      logger::error("Argument can not found ({}) ", str(c.info()));
      return Value::error("Argument can not found");
    }

    Value removeConsumerConnection(const ConnectionInfo& ci) {
      ///未実装
      return ci;
    }

    ConnectionList getProviderConnectionList() const {
      return providerConnectionList_;
    }
    ConnectionList getConsumerConnectionsByArgName(const std::string& argName) const {
      if (consumerConnectionListDictionary_.count(argName) == 0) return ConnectionList();
      return consumerConnectionListDictionary_.at(argName);
    }

    bool hasConsumerConnection(const ConnectionInfo& ci) const {
      for(const auto v : getConsumerConnectionsByArgName(ci.at("consumer").at("target").at("name").stringValue())) {
        if (v.info().at("provider") == ci.at("provider")) 
            return true;
      }
      return false;
    }

    bool hasProviderConnection(const ConnectionInfo& ci) const {
      for(const auto v : providerConnectionList_) {
        if (v.info().at("consumer") == ci.at("consumer")) return true;
      }
      return false;
    }
  };
  
}
