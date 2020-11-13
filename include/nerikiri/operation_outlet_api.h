#pragma once 

#include <nerikiri/value.h>
#include <nerikiri/connection_api.h>


namespace nerikiri {

  class OperationAPI;
  class ConnectionAPI;
    
  class OperationOutletAPI {
  public:
    virtual ~OperationOutletAPI() {}

  public:
    /*
    virtual Value put(Value&& v) = 0;

    virtual Value invoke() = 0;
    */

    virtual OperationAPI* owner() = 0;

    virtual Value get() const = 0;

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const = 0;

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) = 0;
    
    virtual Value removeConnection(const std::string& _fullName) = 0;

    
    virtual Value info() const = 0;
  };

  class NullOperationOutlet : public OperationOutletAPI {
  private:
    NullOperation owner_;
  public:
    virtual ~NullOperationOutlet() {}
    /*

    virtual Value invoke() override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }

    virtual Value put(Value&& v) override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }
    */
   
    virtual Value get() const override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override {
      logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__);
      return {};
    }

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }
    
    virtual Value removeConnection(const std::string& _fullName) override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }

    virtual OperationAPI* owner() override {
      logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__);
      return &owner_;
    }


    virtual Value info() const override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }
  };
}