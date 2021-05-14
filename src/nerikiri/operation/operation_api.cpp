#include <nerikiri/operation_api.h>
using namespace nerikiri;

class NullOperation : public OperationAPI {
public:
    NullOperation() : OperationAPI("NullOperation", "NullOperation", "null") {}

    virtual ~NullOperation() {}


    virtual Value setOwner(const std::shared_ptr<Object>& obj) override {
        return Value::error(logger::error("NullOperation::setOwner() failed. Operation is null."));
    }

    virtual Value fullInfo() const override { 
        return Value::error(logger::error("NullOperation::{}({}) failed. Operation is null.", __func__));
    }
    
    virtual Value call(const Value& value) override { 
        return Value::error(logger::error("NullOperation::{}({}) failed. Operation is null.", __func__, value));
    }

    virtual Value invoke() override { 
        return Value::error(logger::error("NullOperation::{}({}) failed. Operation is null.", __func__));
    }

    virtual Value execute() override { 
        return Value::error(logger::error("NullOperation::{}({}) failed. Operation is null.", __func__));
    }
    
    virtual std::shared_ptr<OperationOutletAPI> outlet() const override { 
        logger::error("NullOperation::{}() failed. Operation is null.", __func__);
        return nullOperationOutlet();
    }

    virtual std::shared_ptr<OperationInletAPI> inlet(const std::string& name) const override {
        logger::error("NullOperation::{}() failed. Operation is null.", __func__);
        return nullOperationInlet();
    }

    virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override {
        logger::error("NullOperation::{}() failed. Operation is null.", __func__);
        return {};
    }
};

std::shared_ptr<OperationAPI> nerikiri::nullOperation() {
    return std::make_shared<NullOperation>();
}