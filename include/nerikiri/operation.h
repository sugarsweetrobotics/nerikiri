#pragma once

#include <map>
#include <functional>
#include "nerikiri/logger.h"
#include "nerikiri/object.h"
#include "nerikiri/value.h"
#include "nerikiri/connection.h"
#include "nerikiri/functional.h"

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
    OperationBase();
    OperationBase(const OperationBase& op);

    OperationBase(const Value& info);

    virtual ~OperationBase();

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

    virtual Value addProviderConnection(Connection&& c) { 
      return Value::error(logger::error("{} failed. Operation is null", __func__));
    }

    virtual Value addConsumerConnection(Connection&& c) { 
      return Value::error(logger::error("{} failed. Operation is null", __func__));
    }

    virtual Value call(const Value& value) {
       return Value::error(logger::error("{} failed. Operation is null", __func__));
    }

    virtual Value collectValues() const {
       return Value::error(logger::error("{} failed. Operation is null", __func__));
    }

    virtual Value removeProviderConnection(const ConnectionInfo& ci);

    virtual Value removeConsumerConnection(const ConnectionInfo& ci);

    virtual Value getConnectionInfos() const;

    virtual ConnectionList getOutputConnectionList() const;

    virtual Value getOutputConnectionInfos() const;

    virtual Connection getOutputConnection(const Value& ci) const;

    virtual Value getOutputConnectionInfo(const Value& ci) const;

    virtual Value getInputConnectionInfos() const;

    virtual Connection getInputConnection(const Value& conInfo) const;

    virtual Value getInputConnectionInfo(const std::string& arg) const;

    virtual Value getInputConnectionInfo(const std::string& arg, const Value& ci) const;

    virtual ConnectionList getInputConnectionsByArgName(const std::string& argName) const;

    virtual bool hasInputConnectionRoute(const ConnectionInfo& ci) const;

    virtual bool hasInputConnectionName(const ConnectionInfo& ci) const;

    virtual bool hasOutputConnectionRoute(const ConnectionInfo& ci) const;

    virtual bool hasOutputConnectionName(const ConnectionInfo& ci) const;

    virtual Value putToArgument(const std::string& argName, const Value& value);

    virtual Value putToArgumentViaConnection(const Value& conInfo, const Value& value);

    virtual Value getOutput();

    virtual Value push(const Value& ci, Value&& value);
    
    virtual Value execute() ;

    virtual Value invoke();
    
    static std::shared_ptr<OperationBase> null;// = std::make_shared<OperationBase>();
  };

  class Operation : public OperationBase {
  private:
    std::function<Value(const Value&)> function_;
    //F function_;
  public:
    Operation(): OperationBase(), function_(nullptr) {}
    Operation(Value&& info) : OperationBase(std::move(info)), function_(nullptr) {}
    Operation(Value&& info, std::function<Value(const Value&)> func): OperationBase(std::move(info)), function_((func)) {}
    Operation(const Value& info, std::function<Value(const Value&)> func) : OperationBase(info), function_(func) {}

    virtual ~Operation() {}

  public:
    Operation& operator=(const Operation& op) {
      logger::trace("Operation copy");
      operator=(op);
      this->function_ = op.function_;
      return *this;
    }


    virtual Value addProviderConnection(Connection&& c) override;

    virtual Value addConsumerConnection(Connection&& c) override;

    virtual Value call(const Value& value) override {
      outputBuffer_.push(std::move(this->function_(value)));
      return getOutput();
    }
    
    virtual Value collectValues() const override;

    virtual Value invoke() override {
      return call(collectValues());
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



}
