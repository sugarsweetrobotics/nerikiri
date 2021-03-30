#include <nerikiri/ec_api.h>
#include <nerikiri/ec_state_api.h>




using namespace nerikiri;



class NullECState : public ECStateAPI {
public:
    NullECState() : ECStateAPI("NullECState", "NullECState", "null") {}
    virtual ~NullECState() {}

public:
    virtual Value activate() {
        return Value::error(logger::error("NullECState::{}}() called. ECState is null.", __func__));
    }

};


class NullExecutionContext : public ExecutionContextAPI {
public:
    NullExecutionContext() : ExecutionContextAPI("ExecutionContext", "NullExecutionContext", "null") {}
    virtual ~NullExecutionContext() {}

public:
    virtual Value start() override {
        return Value::error(logger::error("NullExecutionContext::start() called. ExecutionContext is null."));
    }

    virtual Value stop() override {
        return Value::error(logger::error("NullExecutionContext::stop() called. ExecutionContext is null."));
    }

    virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override {
        logger::error("NullExecutionContext::operations() called. ExecutionContext is null.");
        return {};
    }

    virtual Value bind(const std::shared_ptr<OperationAPI>& op) override {
        return Value::error(logger::error("NullExecutionContext::{}}() called. ExecutionContext is null.", __func__));
    }

    virtual Value unbind(const std::string& fullName) override {
        return Value::error(logger::error("NullExecutionContext::{}}() called. ExecutionContext is null.", __func__));
    }

    virtual std::shared_ptr<ECStateAPI> startedState() const override {
        return std::make_shared<NullECState>();
    }

    virtual std::shared_ptr<ECStateAPI> stoppedState() const override {
        return std::make_shared<NullECState>();
    }
};

std::shared_ptr<ExecutionContextAPI> nerikiri::nullEC() {
    return std::make_shared<NullExecutionContext>();
}

std::shared_ptr<ECStateAPI> nerikiri::nullECState() {
    return std::make_shared<NullECState>();
}