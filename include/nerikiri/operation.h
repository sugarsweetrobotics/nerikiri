#pragma once

#include <map>
#include <functional>
#include "nerikiri/logger.h"
#include "nerikiri/valuemap.h"
#include "nerikiri/operationinfo.h"
#include "nerikiri/connection.h"

namespace nerikiri {
  //class OperationBase;
  class Process;
  
  using Process_ptr = Process*;

  inline bool operationNameValidator(const std::string& name) {
    if (name.find("/") >= 0) {
      return false; // Invalid Name
    }
    return true;
  };

  inline bool operationValidator(const Value& opinfo) {
    if (!operationNameValidator(opinfo.at("name").stringValue())) {
      return false;
    }
    return true;
  }
  
  class Invokable {
  public:
    virtual Value invoke() const = 0;
  };

  class Callable {
  public:
    virtual Value call(Value&& value) const = 0;
  };

  Value call_operation(const Callable& operation, Value&& value);
 
  Value invoke_operation(const Invokable& operation);

  template<typename F>
  class OperationBase {
  protected:
    Process_ptr process_;
    bool is_null_;
    OperationInfo info_;
    F function_;
    ConnectionList providerConnectionList_;
    ConnectionListDictionary consumerConnectionListDictionary_;

  public:
    
  public:
    OperationBase() : process_(nullptr), is_null_(true), info_(Value::error("null operation")) {
      logger::trace("OperationBase construct with ()");
      
    }

    OperationBase(OperationInfo&& info) : is_null_(true), info_(std::move(info)) {
      logger::trace("OperationBase construct with (info)");
      if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
      }
    }

    OperationBase(const OperationBase& op): process_(op.process_), function_(op.function_), info_(op.info_), is_null_(op.is_null_),
      providerConnectionList_(op.providerConnectionList_), consumerConnectionListDictionary_(op.consumerConnectionListDictionary_) {
      logger::trace("OperationBase copy construction."); 
      if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
      }
    }

    OperationBase(OperationInfo&& info, F&& func):
      info_(info), function_(func), is_null_(false) {
      logger::trace("OperationBase::OperationBase({})", str(info));
      if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
      }
      info_.at("defaultArg").object_for_each([this](const std::string& key, const Value& value) -> void{
          consumerConnectionListDictionary_.emplace(key, ConnectionList());
      });
    }

    OperationBase(const OperationInfo& info, F&& func):
      info_(info), function_(func), is_null_(false) {
      logger::trace("OperationBase::OperationBase({})", str(info));
      if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
      }
      info_.at("defaultArg").object_for_each([this](const std::string& key, const Value& value) -> void{
          consumerConnectionListDictionary_.emplace(key, ConnectionList());
      });
    }

    virtual ~OperationBase() {
      logger::trace("Operation desctructed.");
    }

    OperationBase& operator=(const OperationBase& op) {
      logger::trace("Operation copy");
      process_ = op.process_;
      function_ = op.function_;
      info_ = op.info_;
      is_null_ = op.is_null_;
      providerConnectionList_ = op.providerConnectionList_;
      consumerConnectionListDictionary_ = op.consumerConnectionListDictionary_;
      return *this;
    }
  public:
    const OperationInfo& info() const { return info_; }

    bool isNull() const { return is_null_; }

    Value addProviderConnection(Connection&& c) {
      logger::trace("OperationBase::addProviderConnection({})", str(c.info()));
      providerConnectionList_.emplace_back(std::move(c));
      return c.info();
    }

    Value removeProviderConnection(const ConnectionInfo& ci) {
      logger::trace("OperationBase::removeProviderConnection({})", str(ci));
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
      logger::trace("Operation::removeConsumerConnection({})", str(ci));
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

    Value collectValues() const {
      return Value(info().at("defaultArg").template object_map<std::pair<std::string, Value>>(
        [this](const std::string& key, const Value& value) -> std::pair<std::string, Value>{
          for(auto& con : getConsumerConnectionsByArgName(key)) {
            if (con.isPull()) { return {key, con.pull()}; }
          }
          return {key, value};
        }
      ));
    }

    
    friend Value call_operation(const Callable& operation, Value&& value);

    friend Value nerikiri::invoke_operation(const Invokable& operation);
  };
  
  class Operation : public OperationBase<std::function<Value(Value)>>, public Callable, public Invokable {
  public:
    Operation(): OperationBase<std::function<Value(Value)>>() {}
    Operation(OperationInfo&& info) : OperationBase(std::move(info)) {} //is_null_(true), info_(std::move(info)) {}
    Operation(OperationInfo&& info, std::function<Value(Value)>&& func): OperationBase<std::function<Value(Value)>>(std::move(info), std::move(func)) {}
    Operation(const OperationInfo& info, std::function<Value(Value)>&& func) : OperationBase<std::function<Value(Value)>>(info, std::move(func)) {}

    virtual ~Operation() {}

  public:
    Operation& operator=(const Operation& op) {
      logger::trace("Operation copy");
      operator=(op);

      return *this;
    }


    virtual Value call(Value&& value) const override {
      if (this->isNull()) return Value::error("Opertaion is null");
      return this->function_(value);
    }

    
    virtual Value invoke() const override {
      if (isNull()) {
        return Value::error("Opertaion is null");
      }
      return nerikiri::call_operation(*this, collectValues());
    }

    static Operation null;
  };
}
