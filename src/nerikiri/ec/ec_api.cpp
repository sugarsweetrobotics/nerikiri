#include <nerikiri/ec_api.h>
#include <nerikiri/logger.h>

using namespace nerikiri;

class NullExecutionContext : public ExecutionContextAPI {
public:
    NullExecutionContext() : ExecutionContextAPI() {}
    virtual ~NullExecutionContext() {}


    virtual bool onStarting() override {
        logger::error("NullExecutionContext::{}() called. ExecutionContext is null.", __func__);
        return false;
    }
    virtual bool onStarted() override {
        logger::error("NullExecutionContext::{}() called. ExecutionContext is null.", __func__);
        return false;
    }

    virtual bool onStopping() override {
        logger::error("NullExecutionContext::{}() called. ExecutionContext is null.", __func__);
        return false;
    }
    virtual bool onStopped() override {
        logger::error("NullExecutionContext::{}() called. ExecutionContext is null.", __func__);
        return false;
    }
    virtual bool svc() override {
        logger::error("NullExecutionContext::{}() called. ExecutionContext is null.", __func__);
        return false;
    }
};

std::shared_ptr<ExecutionContextAPI> nerikiri::nullEC() {
    return std::make_shared<NullExecutionContext>();
}