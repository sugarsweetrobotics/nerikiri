#include <nerikiri/operation_api.h>
#include <nerikiri/operation_outlet_api.h>
#include <nerikiri/operation_inlet_api.h>

using namespace nerikiri;

class NullOperation : public OperationAPI {
public:
    NullOperation() : OperationAPI("NullOperation", "NullOperation", "null") {}

    virtual ~NullOperation() {}

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
        return std::make_shared<NullOperationOutlet>();
    }

    virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override {
        logger::error("NullOperation::{}() failed. Operation is null.", __func__);
        return {};
    }
};

std::shared_ptr<OperationAPI> nerikiri::nullOperation() {
    return std::make_shared<NullOperation>();
}