#include <nerikiri/operation_outlet_api.h>


using namespace nerikiri;



  class NullOperationOutlet : public OperationOutletAPI {
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
      return owner_.get();
    }


    virtual Value info() const override {
      return Value::error(logger::error("NullOperationOutlet::{}() failed. OperationOutlet is null.", __func__));
    }
  };

  
  std::shared_ptr<OperationOutletAPI> nerikiri::nullOperationOutlet() { return std::make_shared<NullOperationOutlet>(); }
