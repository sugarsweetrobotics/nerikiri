#pragma once

#include <map>
#include <functional>
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
    OperationBase() : process_(nullptr), Object() {}

    OperationBase(const OperationBase& op);

    OperationBase(const Value& info);

    virtual ~OperationBase() {}

    OperationBase& operator=(const OperationBase& op) {
      //logger::trace("Operation copy");
      process_ = op.process_;
      info_ = op.info_;
      is_null_ = op.is_null_;
      outputConnectionList_ = op.outputConnectionList_;
      inputConnectionListDictionary_ = op.inputConnectionListDictionary_;
      bufferMap_ = op.bufferMap_;
      return *this;
    }
  
  public:

    Value addProviderConnection(Connection&& c);

    Value addConsumerConnection(Connection&& c);

    virtual Value call(const Value& value) {
       return Value::error("OperationBase::call() failed. Caller Operation is null.");
    };

    Value collectValues() const;

    Value execute() ;

    Value getConnectionInfos() const;

    Value getInputConnectionInfos() const;

    Connection getInputConnection(const Value& conInfo) const;

    Value getInputConnectionInfo(const std::string& arg) const;

    Value getInputConnectionInfo(const std::string& arg, const Value& ci) const;

    ConnectionList getInputConnectionsByArgName(const std::string& argName) const;

    ConnectionList getOutputConnectionList() const;

    Value getOutputConnectionInfos() const;

    Connection getOutputConnection(const Value& ci) const;

    Value getOutput();

    Value getOutputConnectionInfo(const Value& ci) const;

    bool hasInputConnectionRoute(const ConnectionInfo& ci) const;

    bool hasInputConnectionName(const ConnectionInfo& ci) const;

    bool hasOutputConnectionRoute(const ConnectionInfo& ci) const;

    bool hasOutputConnectionName(const ConnectionInfo& ci) const;

    Value invoke();

    Value push(const Value& ci, Value&& value);
    
    Value putToArgument(const std::string& argName, const Value& value);

    Value putToArgumentViaConnection(const Value& conInfo, const Value& value);

    Value removeProviderConnection(const ConnectionInfo& ci);

    Value removeConsumerConnection(const ConnectionInfo& ci);

    
    static std::shared_ptr<OperationBase> null;// = std::make_shared<OperationBase>();
  };



inline OperationBase::OperationBase(const Value& info):
    Object(info) {
    //logger::trace("OperationBase::OperationBase({})", str(info));
    if (!operationValidator(info_)) {
        is_null_ = true;//logger::error("OperationInformation is invalid.");
    } else {
    info_.at("defaultArg").object_for_each([this](const std::string& key, const Value& value) -> void{
        inputConnectionListDictionary_.emplace(key, ConnectionList());
        bufferMap_.emplace(key, std::make_shared<NewestValueBuffer>(info_.at("defaultArg").at(key)));
    });
    }
}


}