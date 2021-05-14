#pragma once

#include <nerikiri/operation_api.h>
#include "newest_value_buffer.h"
#include "connection_container.h"
namespace nerikiri {


  class OperationOutletBase  : public OutletAPI {
  public:
    OperationOutletBase(OperationAPI* operation): operation_(operation) {}
    virtual ~OperationOutletBase() {}

  public:
    OperationAPI* operation_;

    NewestValueBuffer outputBuffer_;
    ConnectionContainer connections_;
  public:
    virtual OperationAPI* owner() { return operation_; }

    virtual std::string ownerFullName() const override { return operation_->fullName(); }

    virtual Value invokeOwner() override { return operation_->invoke(); }

    virtual Value get() const override { return outputBuffer_.pop(); }

    virtual Value invoke() { 
      outputBuffer_.push(operation_->invoke());
      return outputBuffer_.pop(); 
    }

    virtual Value info() const override {
        return {
            {"ownerFullName", operation_->fullName()},
            {"operation", operation_->info()}
        };
    }

    virtual Value fullInfo() const override {
        return {
            {"ownerFullName", operation_->fullName()},
            {"connections", {
                nerikiri::functional::map<Value, std::shared_ptr<ConnectionAPI>>(connections_.connections(), [](auto c) { return c->info(); })
            }}//,
            //{"operation", operation_->info()}
        };
    }

    virtual Value put(Value&& v);

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override { return connections_.connections(); }
    
    virtual Value connectTo(const std::shared_ptr<InletAPI>& inlet, const Value& connectionInfo) override;
    
    virtual Value disconnectFrom(const std::shared_ptr<InletAPI>& inlet) override;
    /*
    virtual Value removeConnection(const std::string& _fullName) override {
      return connections_.removeConnection(_fullName);
    }
    */
  };
}