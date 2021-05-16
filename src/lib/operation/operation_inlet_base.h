#pragma once


#include <map>
#include <functional>
#include <thread>
#include <mutex>

#include <juiz/juiz.h>
#include <juiz/object.h>
#include <juiz/operation_api.h>

#include <juiz/connection.h>
#include <juiz/utils/functional.h>

#include "newest_value_buffer.h"
#include "connection_container.h"
#include "../operation/operation_base.h"

namespace juiz {

  class OperationInletBase : public InletAPI {
  protected:
    const std::string name_;
    const Value default_;
    OperationAPI* operation_;
    bool argument_updated_;

    ConnectionContainer connections_;
    std::shared_ptr<NewestValueBuffer> buffer_;
    std::mutex argument_mutex_;
  public:
    OperationInletBase(const std::string& name, OperationAPI* operation, const Value& defaultValue);
    virtual ~OperationInletBase() {}

  public:
    //virtual OperationAPI* owner() override { return operation_; }
    virtual Value executeOwner() override { 
      logger::trace("OperationInletBase::executeOwner() called");
      return operation_->execute(); 
    }

    virtual bool isNull() const override {
        return false;
    }
    
    virtual std::string name() const override { return name_; }

    virtual std::string ownerFullName() const override { 
      return operation_->fullName();
    }

    virtual Value defaultValue() const override { return default_; }

    virtual Value get() const override { return buffer_->pop(); }

    virtual Value info() const override;

    virtual Value fullInfo() const override;

    virtual Value put(const Value& value) override;

    virtual Value execute(const Value& value){
      put(value);
      return operation_->execute();
    }

    virtual Value collectValues();

    virtual bool isUpdated() const override { 
      if(argument_updated_) return true;
      // どの接続もPull型でないならupdateしない
      return juiz::functional::for_all<std::shared_ptr<ConnectionAPI>>(connections(), [](auto c) { return !c->isPull(); }); 
    }

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override { return connections_.connections(); }

    virtual Value connectTo(const std::shared_ptr<OutletAPI>& outlet, const Value& connectionInfo) override;
    
    virtual Value disconnectFrom(const std::shared_ptr<OutletAPI>& outlet) override;

    //virtual Value removeConnection(const std::string& _fullName) override {
    //  return connections_.removeConnection(_fullName);
  };

}