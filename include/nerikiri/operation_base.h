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

  class ConnectionContainer {
  private:
    std::vector<std::shared_ptr<ConnectionAPI>> connections_;
  public:
    ~ConnectionContainer() {}

  public:
    std::vector<std::shared_ptr<ConnectionAPI>> connections() const { return connections_; }

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& con) {
      for(auto c : connections()) {
        if (c->fullName() == con->fullName()) {
          return Value::error(logger::error("ConnectionContainer::addConnection({}) failed. Outlet already has the same name connection.", con->fullName()));
        }
      }
      return con->info();
    }

    Value removeConnection(const std::string& _fullName) {
      for(auto it = connections_.begin(); it != connections_.end();++it) {
        if ((*it)->fullName() == _fullName) {
          connections_.erase(it);
          return (*it)->info();
        }
      }
      return Value::error(logger::warn("ConnectionContainer::removeConnection({}) failed. Connection not found.", _fullName));
    }
  };

  class OperationOutletBase  : public OperationOutletAPI {
  public:
    OperationOutletBase(OperationAPI* operation): operation_(operation) {}
    virtual ~OperationOutletBase() {}

  public:
    OperationAPI* operation_;

    NewestValueBuffer outputBuffer_;
    ConnectionContainer connections_;
  public:
  
    virtual Value get() const  { return outputBuffer_.pop(); }

    virtual Value invoke() { 
      outputBuffer_.push(operation_->invoke());
      return outputBuffer_.pop(); 
    }

    virtual Value put(Value&& v);

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override { return connections_.connections(); }

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& con) override {
      return connections_.addConnection(con);
    }
    
    virtual Value removeConnection(const std::string& _fullName) override {
      return connections_.removeConnection(_fullName);
    }
  };

  class OperationInletBase : public OperationInletAPI {
  public:
    OperationInletBase(const std::string& name, OperationAPI* operation, const Value& defaultValue) 
      : name_(name), operation_(operation), default_(defaultValue), argument_updated_(false) {}

    virtual ~OperationInletBase() {}
  private:
    const std::string name_;
    const Value default_;
    OperationAPI* operation_;
    bool argument_updated_;

    ConnectionContainer connections_;
    std::shared_ptr<NewestValueBuffer> buffer_;
    std::mutex argument_mutex_;

  public:
    virtual std::string name() const override { return name_; }

    virtual Value defaultValue() const override { return default_; }

    virtual Value get() const override {
      return buffer_->pop();
    }

    virtual Value put(const Value& value) override {
      std::lock_guard<std::mutex> lock(argument_mutex_);
      if (!value.isError()) {
        buffer_->push(value);
        argument_updated_ = true;
      }
      return value;
    }

    virtual Value execute(const Value& value){
      put(value);
      return operation_->execute();
    }

    virtual Value collectValues() override;

    virtual bool isUpdated() const override { 
      if(argument_updated_) return true;
      // どの接続もPull型でないならupdateしない
      return nerikiri::functional::for_all<std::shared_ptr<ConnectionAPI>>(connections(), [](auto c) { return !c->isPull(); }); 
    }

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override { return connections_.connections(); }

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& con) override {
      return connections_.addConnection(con);
    }
    
    virtual Value removeConnection(const std::string& _fullName) override {
      return connections_.removeConnection(_fullName);
    }
  };


  class OperationBase : public OperationAPI {
  protected:
    const std::string& operationTypeName_;
    std::vector<std::shared_ptr<OperationInletBase>> inlets_;
    std::shared_ptr<OperationOutletBase> outlet_;
  public:

    virtual std::shared_ptr<OperationOutletAPI> outlet() const override { return outlet_; }

    virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override { return {inlets_.begin(), inlets_.end()}; }
      
  public:
    OperationBase(const std::string& typeName, const std::string& operationTypeName, const std::string& fullName, const Value& defaultArg = {});

    // OperationBase(const OperationBase& op);

    virtual ~OperationBase();

  public:

    virtual std::string operationTypeName() const override { return operationTypeName_; }

    virtual Value invoke() override;

	  virtual Value execute() override;
  };
}
