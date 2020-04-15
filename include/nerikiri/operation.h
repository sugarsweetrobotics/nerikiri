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

  class OperationBase : public Object {
  protected:
    Process_ptr process_;
    ConnectionList outputConnectionList_;
    ConnectionListDictionary inputConnectionListDictionary_;
    std::map<std::string, std::shared_ptr<NewestValueBuffer>> bufferMap_;
    NewestValueBuffer outputBuffer_;
  public:
    
  public:
    OperationBase() : process_(nullptr), Object() {
      logger::trace("OperationBase construct with ()");
      
    }

    OperationBase(const OperationBase& op): process_(op.process_), Object(op.info_),
      outputConnectionList_(op.outputConnectionList_), inputConnectionListDictionary_(op.inputConnectionListDictionary_), bufferMap_(op.bufferMap_) {
      logger::trace("OperationBase copy construction."); 
      if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
      }
    }

    OperationBase(const Value& info):
      Object(info) {
      logger::trace("OperationBase::OperationBase({})", str(info));
      if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
      }
      info_.at("defaultArg").object_for_each([this](const std::string& key, const Value& value) -> void{
          inputConnectionListDictionary_.emplace(key, ConnectionList());
          bufferMap_.emplace(key, std::make_shared<NewestValueBuffer>(info_.at("defaultArg").at(key)));
      });
    }

    virtual ~OperationBase() {
      logger::trace("Operation desctructed.");
    }

    OperationBase& operator=(const OperationBase& op) {
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
    
    virtual Value execute() {
      if (isNull()) {
        return Value::error(logger::error("OperationBase::execute() failed. Operation is null"));
      }
      logger::trace("Operation({})::execute()", str(info()));
      auto v = this->invoke();
      for(auto& c : outputConnectionList_) {
        c.putToArgumentViaConnection(v);
      }
      return v;
    }



    virtual Value call(const Value& value) {
      return Value::error("Opertaion is null");
    }

    
    virtual Value invoke() {
      return Value::error("Opertaion is null");
    }

    static std::shared_ptr<OperationBase> null;
  };


  class Operation : public OperationBase {
  private:
    std::function<Value(const Value&)> function_;
  public:
    Operation(): OperationBase() {}
    Operation(Value&& info) : OperationBase(std::move(info)) {}
    Operation(Value&& info, std::function<Value(const Value&)>&& func): OperationBase(std::move(info)), function_(std::move(func)) {}
    Operation(const Value& info, std::function<Value(const Value&)>& func) : OperationBase(info), function_(func) {}

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

  };

}
