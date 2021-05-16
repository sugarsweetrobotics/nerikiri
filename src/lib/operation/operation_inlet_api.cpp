
#include <juiz/operation_api.h>
#include <juiz/logger.h>

using namespace juiz;



class NullOperationInlet : public InletAPI {
private:
    std::shared_ptr<OperationAPI> owner_;
public:
    NullOperationInlet() : owner_(nullOperation()) {}
    virtual ~NullOperationInlet() {}

    virtual bool isNull() const override {
        return true;
    }

    virtual std::string ownerFullName() const override { 
      logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__);
      return "null";
    }
    
    virtual std::string name() const override {
        logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__);
        return "null";
    }

    virtual Value defaultValue() const override {
        return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }

    virtual Value info() const override {
        return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }

    virtual Value get() const override {
        return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }

    virtual Value put(const Value& value) override {
        return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }

    /*
    virtual Value execute(const Value& value) override {
        return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }


    virtual Value collectValues() override {
        return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }

    */
    virtual bool isUpdated() const override {
        logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__);
        return false;
    }

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override {
        logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__);
        return {};
    }

    //virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) override {
    //    return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    //}

    virtual Value connectTo(const std::shared_ptr<OutletAPI>& outlet, const Value& connectionInfo) override {
        return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }

    virtual Value disconnectFrom(const std::shared_ptr<OutletAPI>& outlet) override {
        return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }

    //virtual Value removeConnection(const std::string& _fullName) override {
    //    return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    //}

    virtual Value executeOwner() override {
        return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }
};

std::shared_ptr<InletAPI> juiz::nullOperationInlet() { return std::make_shared<NullOperationInlet>(); }