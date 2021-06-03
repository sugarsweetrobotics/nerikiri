#include <juiz/ec_api.h>
#include <juiz/logger.h>
#include <juiz/operation_api.h>

#include <juiz/connection_api.h>

#include <juiz/ec.h>

using namespace juiz;

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

    virtual void setSvcOperation(const std::shared_ptr<OperationAPI>& op) override { 
        logger::error("NullExecutionContext::{}() called. ExecutionContext is null.", __func__);
        return;
    }

};

std::shared_ptr<ExecutionContextAPI> juiz::nullEC() {
    return std::make_shared<NullExecutionContext>();
}


bool ExecutionContextBase::svc() { 
    logger::trace("ExecutionContextBase::svc() called");
    for(auto c : svcOperation_->outlet()->connections()) {
        c->inlet()->put({});
    }
    return true;
    //return !svcOperation_->execute().isError(); 
}