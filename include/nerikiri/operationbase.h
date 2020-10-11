#pragma once

#include <map>
#include <functional>
#include <thread>
#include <mutex>

#include "nerikiri/nerikiri.h"
#include "nerikiri/object.h"
#include "nerikiri/operation_api.h"

#include "nerikiri/connection.h"
#include "nerikiri/functional.h"

#include "nerikiri/newest_value_buffer.h"
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
    if (!nameValidator(opinfo.at("typeName").stringValue())) {
      return false;
    }
    return true;
  }
  




  class OperationBase : public Object, public OperationAPI {
  protected:
      ConnectionList outputConnectionList_;
      ConnectionListDictionary inputConnectionListDictionary_;
      std::map<std::string, std::shared_ptr<NewestValueBuffer>> bufferMap_;
      NewestValueBuffer outputBuffer_;

      std::mutex argument_mutex_;
      bool argument_updated_;
  public:

  public:
      OperationBase() : Object(), argument_updated_(true) {}

      OperationBase(const OperationBase& op);

      OperationBase(const Value& info);

      virtual ~OperationBase() {}

      OperationBase& operator=(const OperationBase& op) {
          //logger::trace("Operation copy");
          //process_ = op.process_;
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

      virtual Value call(const Value& value) override {
          return Value::error("OperationBase::call() failed. Caller Operation is null.");
      };

      Value collectValues();

	    virtual Value execute() override;

    Value getConnectionInfos() const;

    Value getInputConnectionInfos() const;

    Connection getInputConnection(const std::string& argName, const std::string& conName) const;

    Value getInputConnectionInfo(const std::string& arg) const;

    Value getInputConnectionInfo(const std::string& arg, const Value& ci) const;

    ConnectionList getInputConnectionsByArgName(const std::string& argName) const {
        if (inputConnectionListDictionary_.count(argName) == 0) return ConnectionList();
        return inputConnectionListDictionary_.at(argName);
    }

    ConnectionList getOutputConnectionList() const;

    Value getOutputConnectionInfos() const;

    Connection getOutputConnection(const std::string& name) const;

    Value getOutput();

    Value getOutputConnectionInfo(const Value& ci) const;

    bool hasInputConnectionRoute(const ConnectionInfo& ci) const;

    bool hasInputConnectionName(const ConnectionInfo& ci) const;

    bool hasOutputConnectionRoute(const ConnectionInfo& ci) const;

    bool hasOutputConnectionName(const ConnectionInfo& ci) const;

    Value invoke();

    Value push(const Value& ci, Value&& value);
    
    Value putToArgument(const std::string& argName, const Value& value);

    Value putToArgumentViaConnection(const std::string& argName, const std::string& conName, const Value& value);

    Value removeProviderConnection(const ConnectionInfo& ci);

    Value removeConsumerConnection(const std::string& targetArgName, const std::string& conName);


    
    //static std::shared_ptr<OperationBase> null;// = std::make_shared<OperationBase>();
  };

inline std::shared_ptr<OperationBase> nullOperation() {
  return std::make_shared<OperationBase>();
}




}
