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

#include "nerikiri/operation_inlet_base.h"

namespace nerikiri {

  class Process;

  using Process_ptr = Process*;


  class OperationOutletBase  : public OperationOutletAPI {
  public:
    OperationOutletBase(OperationAPI* operation): operation_(operation) {}
    virtual ~OperationOutletBase() {}

  public:
    OperationAPI* operation_;

    NewestValueBuffer outputBuffer_;
    ConnectionContainer connections_;
  public:
    virtual OperationAPI* owner() override { return operation_; }
  

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
      return connections_.addConnection(con);
    }
    
    virtual Value removeConnection(const std::string& _fullName) override {
      return connections_.removeConnection(_fullName);
    }
  };


  class OperationBase : public OperationAPI {
  protected:
    //const std::string& operationTypeName_;
    std::vector<std::shared_ptr<OperationInletBase>> inlets_;
    std::shared_ptr<OperationOutletBase> outlet_;
  public:

    virtual Value fullInfo() const override;

    virtual std::shared_ptr<OperationOutletAPI> outlet() const override { return outlet_; }

    virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override { return {inlets_.begin(), inlets_.end()}; }
      
  public:
    OperationBase(const std::string& className, const std::string& typeName, const std::string& fullName, const Value& defaultArg = {});

    // OperationBase(const OperationBase& op);

    virtual ~OperationBase();

  public:

    //virtual std::string operationTypeName() const override { return operationTypeName_; }

    virtual Value invoke() override;

	  virtual Value execute() override;
  };
}
