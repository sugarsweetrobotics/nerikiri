#pragma once

#include <map>
#include <functional>
#include <thread>
#include <mutex>

#include "nerikiri/nerikiri.h"
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
    if (!nameValidator(opinfo.at("typeName").stringValue())) {
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
    virtual Value& popRef() { return empty_ ? defaultValue_ : buffer_; }
    virtual bool isEmpty() const { return empty_; }
  };

  class OperationBase : public Object {
  protected:
      //Process_ptr process_;
      ConnectionList outputConnectionList_;
      ConnectionListDictionary inputConnectionListDictionary_;
      std::map<std::string, std::shared_ptr<NewestValueBuffer>> bufferMap_;
      NewestValueBuffer outputBuffer_;

      std::mutex argument_mutex_;
      bool argument_updated_;
  public:

  public:
      OperationBase() : Object(), argument_updated_(false) {}

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

      virtual Value call(const Value& value) {
          return Value::error("OperationBase::call() failed. Caller Operation is null.");
      };

      Value collectValues() {
        if (isNull()) { return Value::error("OperationBase::collectValues() failed. Caller Operation is null."); }

        std::lock_guard<std::mutex> lock(argument_mutex_);
        return Value(info().at("defaultArg").template object_map<std::pair<std::string, Value>>(
          [this](const std::string& key, const Value& value) -> std::pair<std::string, Value> {
            if (!bufferMap_.at(key)->isEmpty()) {
              return { key, bufferMap_.at(key)->popRef() };
            }
            for (auto& con : getInputConnectionsByArgName(key)) {
              if (con.isPull()) { return { key, con.pull() }; }
            }
            return { key, value };
          }
        ));
    }

	Value execute() {
		  if (isNull()) { return Value::error("OperationBase::execute() failed. Caller Operation is null."); }
		  Value&& v = this->invoke();
	
  	  for (auto& c : outputConnectionList_) {
			  c.putToArgumentViaConnection(v);
		  }
		  return v;

	}

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

    Value invoke() {
        if (isNull()) { return Value::error("OperationBase::invoke() failed. Caller Operation is null."); }
        try {
          return call(collectValues());
        } catch (const std::exception& ex) {
          return Value::error(std::string("OperationBase::invoke() failed. Exception occurred: ") + ex.what());
        }
    }

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
