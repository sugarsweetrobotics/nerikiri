#pragma once

#include <map>
#include <functional>
#include "nerikiri/logger.h"
#include "nerikiri/value.h"
#include "nerikiri/connection.h"

namespace nerikiri {

  class Process;
  
  using OperationInfo = Value;

  using Process_ptr = Process*;

  inline bool nameValidator(const std::string& name) {
    if (name.find("/") != std::string::npos) {
      return false; // Invalid Name
    }
    if (name.find(":") != std::string::npos) {
      return false; // Invalid Name
    }
    return true;
  };

  inline bool operationValidator(const Value& opinfo) {
    if (!nameValidator(opinfo.at("name").stringValue())) {
      return false;
    }
    return true;
  }
  
  

  class Callable {
  public:
    virtual Value call(Value&& value) const = 0;
  };

  class Invokable {
  public:
    virtual Value invoke() const = 0;
  };

  Value call_operation(const Callable& operation, Value&& value);
 
  Value invoke_operation(const Invokable& operation);

  class Buffer {
  private:
    bool empty_;
    Value buffer_;
    Value defaultValue_;
  public:
    Buffer(): empty_(true) {}
    Buffer(const Value& defaultValue) : buffer_(defaultValue), defaultValue_(defaultValue), empty_(true) {}

    virtual ~Buffer() {}
  public:
    virtual Value push(Value&& v) { buffer_ = std::move(v); empty_ = false; return buffer_; }
    virtual Value pop() { return empty_ ? defaultValue_ : buffer_; }
    virtual bool isEmpty() const { return empty_; }
  };

  class OperationBaseBase : public Callable, public Invokable {
  protected:
    Process_ptr process_;
    bool is_null_;
    OperationInfo info_;
    ConnectionList providerConnectionList_;
    ConnectionListDictionary consumerConnectionListDictionary_;
    std::map<std::string, std::shared_ptr<Buffer>> bufferMap_;
  public:
    
  public:
    OperationBaseBase() : process_(nullptr), is_null_(true), info_(Value::error("null operation")) {
      logger::trace("OperationBase construct with ()");
      
    }

    OperationBaseBase(OperationInfo&& info) : is_null_(true), info_(std::move(info)) {
      logger::trace("OperationBase construct with (info)");
      if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
      }
      info_.at("defaultArg").object_for_each([this](const std::string& key, const Value& value) -> void{
          consumerConnectionListDictionary_.emplace(key, ConnectionList());
          bufferMap_.emplace(key, std::make_shared<Buffer>(info_.at("defaultArg").at(key)));
      });
    }

    OperationBaseBase(const OperationBaseBase& op): process_(op.process_), info_(op.info_), is_null_(op.is_null_),
      providerConnectionList_(op.providerConnectionList_), consumerConnectionListDictionary_(op.consumerConnectionListDictionary_), bufferMap_(op.bufferMap_) {
      logger::trace("OperationBase copy construction."); 
      if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
      }
    }

    OperationBaseBase(const OperationInfo& info):
      info_(info), is_null_(false) {
      logger::trace("OperationBase::OperationBase({})", str(info));
      if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
      }
      info_.at("defaultArg").object_for_each([this](const std::string& key, const Value& value) -> void{
          consumerConnectionListDictionary_.emplace(key, ConnectionList());
          bufferMap_.emplace(key, std::make_shared<Buffer>(info_.at("defaultArg").at(key)));
      });
    }

    virtual ~OperationBaseBase() {
      logger::trace("Operation desctructed.");
    }

    OperationBaseBase& operator=(const OperationBaseBase& op) {
      logger::trace("Operation copy");
      process_ = op.process_;
      info_ = op.info_;
      is_null_ = op.is_null_;
      providerConnectionList_ = op.providerConnectionList_;
      consumerConnectionListDictionary_ = op.consumerConnectionListDictionary_;
      bufferMap_ = op.bufferMap_;
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
      const auto argName = ci.at("consumer").at("target").at("name").stringValue();
      auto clist = consumerConnectionListDictionary_[argName];
      for (auto it = clist.begin(); it != clist.end();) {
          if ((*it).info().at("name") == ci.at("name")) {
            it = clist.erase(it);
          } else { ++it; }
      }
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
          if (!bufferMap_.at(key)->isEmpty()) {
            return {key, bufferMap_.at(key)->pop()};
          }
          for(auto& con : getConsumerConnectionsByArgName(key)) {
            if (con.isPull()) { return {key, con.pull()}; }
          }
          return {key, value};
        }
      ));
    }

    Value push(const Value& ci, Value&& value) {
      for (auto& c : consumerConnectionListDictionary_[ci.at("target").at("name").stringValue()]) {
        if (c.info().at("name") == ci.at("name")) {
          return bufferMap_[ci.at("target").at("name").stringValue()]->push(std::move(value));
        }
      }
      return Value::error(logger::error("OperationBase::push(Value) can not found connection ({})", str(ci)));
    }
    
    friend Value call_operation(const Callable& operation, Value&& value);

    friend Value nerikiri::invoke_operation(const Invokable& operation);
  };


  template<typename F>
  class OperationBase : public OperationBaseBase {
  protected:
    F function_;
  public:
    
  public:
    OperationBase() : OperationBaseBase() {}

    OperationBase(OperationInfo&& info) : OperationBaseBase(std::move(info)) {}

    OperationBase(const OperationBase& op): OperationBaseBase(op), function_(op.function_) { }

    OperationBase(OperationInfo&& info, F&& func): OperationBaseBase(std::move(info)), function_(func) {}

    OperationBase(const OperationInfo& info, F&& func): OperationBaseBase(info), function_(func) {}

    virtual ~OperationBase() {
      logger::trace("Operation desctructed.");
    }
  };
  
  class Operation : public OperationBase<std::function<Value(Value)>> {
  public:
    Operation(): OperationBase<std::function<Value(Value)>>() {}
    Operation(OperationInfo&& info) : OperationBase(std::move(info)) {}
    Operation(OperationInfo&& info, std::function<Value(Value)>&& func): OperationBase<std::function<Value(Value)>>(std::move(info), std::move(func)) {}
    Operation(const OperationInfo& info, std::function<Value(Value)>&& func) : OperationBase<std::function<Value(Value)>>(info, std::move(func)) {}

    virtual ~Operation() {}

  public:
    Operation& operator=(const Operation& op) {
      logger::trace("Operation copy");
      operator=(op);
      this->function_ = op.function_;
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
