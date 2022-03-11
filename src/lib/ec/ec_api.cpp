#include <juiz/ec_api.h>
#include <juiz/logger.h>
#include <juiz/operation_api.h>

#include <juiz/connection_api.h>

#include <juiz/ec.h>
#include <juiz/process_store.h>
#include <juiz/container_api.h>

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
    //for(auto c : svcOperation_->outlet()->connections()) {
    //    // c->inlet()->put({});
    //    c->inlet()->executeOwner();
    //}
    svcOperation_->execute();
    return true;
    //return !svcOperation_->execute().isError(); 
}



Value juiz::bind(ProcessStore& store, const Value& ecInfo, const Value& opInfo) {
    const std::string ecName = getStringValue(ecInfo["fullName"], "");
    const auto activate_started_ope = store.get<ContainerAPI>(ecName)->operation("activate_state_started.ope");
    const auto opProxy =  store.operationProxy(opInfo);
    return juiz::connect(coreBroker(store), ecName + "_bind_" + opProxy->fullName(), opProxy->inlet("__event__"), activate_started_ope->outlet(), {}); 
}