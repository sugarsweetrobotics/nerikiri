#pragma once

#include <map>
#include <functional>
#include "nerikiri/logger.h"
#include "nerikiri/value.h"
#include "nerikiri/connection.h"
#include "nerikiri/functional.h"
#include "nerikiri/object.h"

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
    virtual Value call(const Value& value) = 0;
  };

  class Invokable {
  public:
    virtual Value invoke() = 0;
  };

  class Executable {
  public:
    virtual Value execute() = 0;
  };

  //Value call_operation(const Callable& operation, Value&& value);
 
  //Value invoke_operation(const Invokable& operation);

  class NewestValueBuffer {
  private:
    bool empty_;
    Value buffer_;
    Value defaultValue_;
  public:

    NewestValueBuffer(): empty_(true) {}
    NewestValueBuffer(const Value& defaultValue) : buffer_(defaultValue), defaultValue_(defaultValue), empty_(true) {}

    virtual ~NewestValueBuffer() {}
  public:
    virtual Value push(const Value& v) { buffer_ = (v); empty_ = false; return buffer_; }
    virtual Value push(Value&& v) { buffer_ = std::move(v); empty_ = false; return buffer_; }
    virtual Value pop() { return empty_ ? defaultValue_ : buffer_; }
    virtual bool isEmpty() const { return empty_; }
  };

  class OperationBaseBase : public Callable, public Invokable, public Executable, public Object {
  protected:
    Process_ptr process_;
    bool is_null_;
    //OperationInfo info_;
    ConnectionList outputConnectionList_;
    ConnectionListDictionary inputConnectionListDictionary_;
    std::map<std::string, std::shared_ptr<NewestValueBuffer>> bufferMap_;
    NewestValueBuffer outputBuffer_;
  public:
    
  public:
    OperationBaseBase() : process_(nullptr), is_null_(true), Object(Value::error("null operation")) {
      logger::trace("OperationBase construct with ()");
      
    }

    OperationBaseBase(OperationInfo&& info) : is_null_(true), Object(std::move(info)) {
      logger::trace("OperationBase construct with (info)");
      if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
      }
      info_.at("defaultArg").object_for_each([this](const std::string& key, const Value& value) -> void{
          inputConnectionListDictionary_.emplace(key, ConnectionList());
          bufferMap_.emplace(key, std::make_shared<NewestValueBuffer>(info_.at("defaultArg").at(key)));
      });
    }

    OperationBaseBase(const OperationBaseBase& op): process_(op.process_), Object(op.info_), is_null_(op.is_null_),
      outputConnectionList_(op.outputConnectionList_), inputConnectionListDictionary_(op.inputConnectionListDictionary_), bufferMap_(op.bufferMap_) {
      logger::trace("OperationBase copy construction."); 
      if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
      }
    }

    OperationBaseBase(const OperationInfo& info):
      Object(info), is_null_(false) {
      logger::trace("OperationBase::OperationBase({})", str(info));
      if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
      }
      info_.at("defaultArg").object_for_each([this](const std::string& key, const Value& value) -> void{
          inputConnectionListDictionary_.emplace(key, ConnectionList());
          bufferMap_.emplace(key, std::make_shared<NewestValueBuffer>(info_.at("defaultArg").at(key)));
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
      outputConnectionList_ = op.outputConnectionList_;
      inputConnectionListDictionary_ = op.inputConnectionListDictionary_;
      bufferMap_ = op.bufferMap_;
      return *this;
    }
  
  public:
    const OperationInfo& info() const { return info_; }
    bool isNull() const { return is_null_; }

    Value addProviderConnection(Connection&& c) {
      logger::trace("OperationBase::addProviderConnection({})", str(c.info()));
      if (isNull()) return Value::error(logger::error("Operation::addProviderConnection failed. Operation is null"));
      outputConnectionList_.emplace_back(std::move(c));
      return c.info();
    }

    Value addConsumerConnection(Connection&& c) {
      logger::trace("Operation::addConsumerConnection({})", str(c.info()));
      if (isNull()) return Value::error(logger::error("Operation::addConsumerConnection failed. Operation is null"));
      if (c.isNull()) {
        return Value::error(logger::error("addConsumerConnection failed. Given connection is null."));
      }

      if (c.info()["input"]["info"]["name"] != info().at("name")) {
          logger::error("requested connection does not match to this operation.");
      }

      const auto argumentName = c.info()["input"]["target"]["name"].stringValue();
      if (info().at("defaultArg").hasKey(argumentName)) {
        auto inf = c.info();
        inputConnectionListDictionary_[argumentName].emplace_back(std::move(c));
        return inf;
      }
      logger::error("Argument can not found ({}) ", str(c.info()));
      return Value::error("Argument can not found");
    }

    Value removeProviderConnection(const ConnectionInfo& ci) {
      logger::trace("Operation::removeProviderConnection({})", str(ci));
      if (isNull()) return Value::error(logger::error("Operation::removeProviderConnection failed. Operation is null"));
      //auto olist = getOutputConnectionList();
      for(auto it = this->outputConnectionList_.begin();it != outputConnectionList_.end();) {
        if ((*it).info().at("name") == ci.at("name")) {
          it = outputConnectionList_.erase(it);
        } else { ++ it; }
      }
      return ci;
    }

    Value removeConsumerConnection(const ConnectionInfo& ci) {
      logger::trace("Operation::removeConsumerConnection({})", str(ci));
      if (isNull()) return Value::error(logger::error("Operation::removeConsumerConnection failed. Operation is null"));
      const auto argName = ci.at("input").at("target").at("name").stringValue();
      //auto clist = inputConnectionListDictionary_[argName];
      for (auto it = inputConnectionListDictionary_[argName].begin(); it != inputConnectionListDictionary_[argName].end();) {
          if ((*it).info().at("name") == ci.at("name")) {
            it = inputConnectionListDictionary_[argName].erase(it);
          } else { ++it; }
      }
      return ci;
    }

    
    Value getConnectionInfos() const {
      return {
        {"output", getOutputConnectionInfos()},
        {"input",  getInputConnectionInfos()}
      };
    }

    ConnectionList getOutputConnectionList() const {
      return outputConnectionList_;
    }

    Value getOutputConnectionInfos() const {
      return nerikiri::map<Value, nerikiri::Connection>(getOutputConnectionList(), [](const auto& con) { return con.info(); });
    }

    Connection getOutputConnection(const Value& ci) const {
      auto olist = getOutputConnectionList();
      for(auto it = olist.begin();it != olist.end();++it) {
        if ((*it).info().at("name") == ci.at("name")) {
          return (*it);
        }
      }
      return Connection::null;
    }

    Value getOutputConnectionInfo(const Value& ci) const {
      return nerikiri::find_first<Value, nerikiri::Connection>(getOutputConnectionList(), 
          [&ci](const auto& con) -> bool { return con.info().at("name") == ci.at("name"); },
          [](const auto& con) { return con.info(); },
          Value::error("Output Connection can not be found."));
    }

    Value getInputConnectionInfos() const {
      return nerikiri::map<std::pair<std::string, Value>, std::string, nerikiri::ConnectionList>(inputConnectionListDictionary_, [](const auto& k, const ConnectionList& v) -> std::pair<std::string,Value> {
        return {k, nerikiri::map<Value, nerikiri::Connection>(v, [](const Connection& con) -> Value { return con.info(); })};
      });
    }


    Connection getInputConnection(const Value& conInfo) const {
      if (inputConnectionListDictionary_.count(conInfo.at("input").at("target").at("name").stringValue()) == 0) {
        return Connection::null;
      }

      return nerikiri::find_first<Connection, nerikiri::Connection>(inputConnectionListDictionary_.at(conInfo.at("input").at("target").at("name").stringValue()), 
        [&conInfo](const Connection& con) -> bool { return con.info().at("name") == conInfo.at("name"); },
        [](const auto& con) { return con; },
        Connection::null);
    }

    Value getInputConnectionInfo(const std::string& arg) const {
      if (inputConnectionListDictionary_.count(arg) == 0) {
        return Value::error(logger::error("Operation::getInputConnectionInfo({}) failed. No key.", arg));
      }
      return nerikiri::map<Value, nerikiri::Connection>(inputConnectionListDictionary_.at(arg), 
        [](const Connection& con) -> Value { return con.info(); });
    }

    Value getInputConnectionInfo(const std::string& arg, const Value& ci) const {
      auto ret = getInputConnectionInfo(arg);
      if (ret.isError()) return ret;
      return nerikiri::find_first<Value, Value>(ret.listValue(),
        [&ci](const auto& v) { return v.at("name") == ci.at("name"); },
        [](const auto& v) { return v; },
        Value::error("Can not find in getInputConnectionInfo(str, value)")
      );
    }

    ConnectionList getInputConnectionsByArgName(const std::string& argName) const {
      if (inputConnectionListDictionary_.count(argName) == 0) return ConnectionList();
      return inputConnectionListDictionary_.at(argName);
    }

    bool hasInputConnectionRoute(const ConnectionInfo& ci) const {
      for(const auto v : getInputConnectionsByArgName(ci.at("input").at("target").at("name").stringValue())) {
        if (v.info().at("output") == ci.at("output")) 
            return true;
      }
      return false;
    }

    bool hasInputConnectionName(const ConnectionInfo& ci) const {
      for(const auto v : getInputConnectionsByArgName(ci.at("input").at("target").at("name").stringValue())) {
        if (v.info().at("name") == ci.at("name")) 
            return true;
      }
      return false;
    }


    bool hasOutputConnectionRoute(const ConnectionInfo& ci) const {
      for(const auto v : outputConnectionList_) {
        if (v.info().at("input") == ci.at("input")) return true;
      }
      return false;
    }

    bool hasOutputConnectionName(const ConnectionInfo& ci) const {
      for(const auto v : outputConnectionList_) {
        if (v.info().at("name") == ci.at("name")) return true;
      }
      return false;
    }

    Value collectValues() const {
      return Value(info().at("defaultArg").template object_map<std::pair<std::string, Value>>(
        [this](const std::string& key, const Value& value) -> std::pair<std::string, Value>{
          if (!bufferMap_.at(key)->isEmpty()) {
            return {key, bufferMap_.at(key)->pop()};
          }
          for(auto& con : getInputConnectionsByArgName(key)) {
            if (con.isPull()) { return {key, con.pull()}; }
          }
          return {key, value};
        }
      ));
    }

    Value putToArgument(const std::string& argName, const Value& value) {
      if (bufferMap_.count(argName) > 0) {
        bufferMap_[argName]->push(value);
        return value;
      }
      return Value::error(logger::error("OperationBaseBase::putToArgument({}) failed.", argName));
    }

    Value putToArgumentViaConnection(const Value& conInfo, const Value& value) {
      logger::trace("OperationBaseBase::putToArgumentViaConnection()");
      const std::string& argName = conInfo.at("input").at("target").at("name").stringValue();
      const std::string& conName = conInfo.at("name").stringValue();
      auto connection = this->getInputConnection(conInfo);
      if (bufferMap_.count(argName) > 0) {
        bufferMap_[argName]->push(value);
        if (connection.isEvent()) {
          execute();
        }
        return value;
      }
      return Value::error(logger::error("OperationBaseBase::putToArgument({}) failed.", argName));
    }

    Value getOutput() {
      return outputBuffer_.pop();
    }

    Value push(const Value& ci, Value&& value) {
      for (auto& c : inputConnectionListDictionary_[ci.at("target").at("name").stringValue()]) {
        if (c.info().at("name") == ci.at("name")) {
          return bufferMap_[ci.at("target").at("name").stringValue()]->push(std::move(value));
        }
      }
      return Value::error(logger::error("OperationBase::push(Value) can not found connection ({})", str(ci)));
    }
    
    virtual Value execute() override {
      logger::trace("Operation({})::execute()", str(info()));
      auto v = this->invoke();
      for(auto& c : outputConnectionList_) {
        c.putToArgumentViaConnection(v);
      }
      return v;
    }
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

    OperationBase(const OperationInfo& info, F& func): OperationBaseBase(info), function_(func) {}

    virtual ~OperationBase() {
      logger::trace("Operation desctructed.");
    }
  };
  
  class Operation : public OperationBase<std::function<Value(Value)>> {
  public:
    Operation(): OperationBase<std::function<Value(Value)>>() {}
    Operation(OperationInfo&& info) : OperationBase(std::move(info)) {}
    Operation(OperationInfo&& info, std::function<Value(Value)>&& func): OperationBase<std::function<Value(Value)>>(std::move(info), std::move(func)) {}
    Operation(const OperationInfo& info, std::function<Value(Value)>& func) : OperationBase<std::function<Value(Value)>>(info, func) {}

    virtual ~Operation() {}

  public:
    Operation& operator=(const Operation& op) {
      logger::trace("Operation copy");
      operator=(op);
      this->function_ = op.function_;
      return *this;
    }


    virtual Value call(const Value& value) override {
      if (this->isNull()) return Value::error("Opertaion is null");
      outputBuffer_.push(std::move(this->function_(value)));
      return getOutput();
    }

    
    virtual Value invoke() override {
      if (isNull()) {
        return Value::error("Opertaion is null");
      }
      return call(collectValues());
      //return nerikiri::call_operation(*this, collectValues());
    }

    static Operation null;
  };

  class OperationFactory {
  private:
    OperationInfo info_;
    std::function<Value(Value)> function_;
    std::string typeName_;
  public:
    std::string typeName() { return typeName_; }
  public:
    OperationFactory(const OperationInfo& info, std::function<Value(Value)>&& func): info_(info), function_(func) {
      typeName_ = info.at("name").stringValue();
    }
    virtual ~OperationFactory() {}

    std::shared_ptr<Operation> create() {   
        return std::make_shared<Operation>(info_, function_); 
    }
  };
}
