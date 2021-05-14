
#include <nerikiri/logger.h>
#include <nerikiri/operation_api.h>

using namespace nerikiri;



  class NullOperationOutlet : public OutletAPI {
  private:
    std::shared_ptr<OperationAPI> owner_;
  public:
    NullOperationOutlet() : owner_(nullOperation()) {}
    virtual ~NullOperationOutlet() {}
    /*

    virtual Value invoke() override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }

    virtual Value put(Value&& v) override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }
    */
   
    virtual std::string ownerFullName() const override { 
      logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__);
      return "null";
    }


    virtual Value get() const override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override {
      logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__);
      return {};
    }
    /*
    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }
    */

    virtual Value connectTo(const std::shared_ptr<InletAPI>& inlet, const Value& connectionInfo) override {
        return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationInlet is null.", __func__));
    }

    virtual Value disconnectFrom(const std::shared_ptr<InletAPI>& inlet) override {
        return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationInlet is null.", __func__));
    }

    /*
    virtual Value removeConnection(const std::string& _fullName) override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }
    */

    virtual Value invokeOwner() override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }


    virtual Value info() const override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }
  };

  
  std::shared_ptr<OutletAPI> nerikiri::nullOperationOutlet() { return std::make_shared<NullOperationOutlet>(); }
