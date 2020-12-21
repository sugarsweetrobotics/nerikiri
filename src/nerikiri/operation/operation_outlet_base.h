#pragma once

#include <nerikiri/operation_api.h>
#include "newest_value_buffer.h"
#include "connection_container.h"
namespace nerikiri {


  class OperationOutletBase  : public OperationOutletAPI {
  public:
    OperationOutletBase(OperationAPI* operation): operation_(operation) {}
    virtual ~OperationOutletBase() {}

  public:
    OperationAPI* operation_;

    NewestValueBuffer outputBuffer_;
    ConnectionContainer connections_;
  public:
    //virtual OperationAPI* owner() override { return operation_; }
  
    virtual std::string ownerFullName() const override { return operation_->fullName(); }

    virtual Value invokeOwner() override { return operation_->invoke(); }

    virtual Value get() const override { return outputBuffer_.pop(); }

    virtual Value invoke() { 
      outputBuffer_.push(operation_->invoke());
      return outputBuffer_.pop(); 
    }

    virtual Value info() const override {
        return {
            {"connections", {
                nerikiri::functional::map<Value, std::shared_ptr<ConnectionAPI>>(connections_.connections(), [](auto c) { return c->info(); })
            }}
        };
    }

    virtual Value put(Value&& v);

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override { return connections_.connections(); }

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& con) override {
      if (con->isNull()) {
        return Value::error(logger::error("OperationBase::addConnection() failed. Passing connection is null"));
      }
      return connections_.addConnection(con);
    }
    
    virtual Value removeConnection(const std::string& _fullName) override {
      return connections_.removeConnection(_fullName);
    }
  };
}