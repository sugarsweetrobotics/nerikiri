#include "nerikiri/logger.h"

#include "nerikiri/ec.h"
using namespace nerikiri;

class StartedECState : public ECStateAPI {
private:
    ExecutionContextBase* ec_;
public:
    StartedECState(ExecutionContextBase* ec) : ECStateAPI("StartedECState", "StartedECState", "started"), ec_(ec) {}
    virtual ~StartedECState() {}
public:
    virtual Value activate() {
        return ec_->start();
    }
};

class StoppedECState : public ECStateAPI {
private:
    ExecutionContextBase* ec_;
public:
    StoppedECState(ExecutionContextBase* ec) : ECStateAPI("StoppedECState", "StoppedECState", "stopped"), ec_(ec) {}
    virtual ~StoppedECState() {}
public:
    virtual Value activate() {
        return ec_->stop();
    }
};

ExecutionContextBase::ExecutionContextBase(const std::string& typeName, const std::string& fullName) : 
    ExecutionContextAPI(typeName, fullName), startedState_(std::make_shared<StartedECState>(this)), stoppedState_(std::make_shared<StoppedECState>(this)) {
}

ExecutionContextBase::~ExecutionContextBase() { }

Value ExecutionContextBase::start() {
    logger::trace("ExecutionContext({})::start() called", getInstanceName());
    if (info_["state"].stringValue() != "started") {
        onStarting();
        info_["state"] = Value("started");
        if (!onStarted()) {
            stop();
            return Value::error(logger::error("ExecutionContext::start() failed. onStarted callback returns false."));;
        }
        logger::info("ExecutionContext({}) successfully started.", getInstanceName());
    }
    return info();
}

Value ExecutionContextBase::stop() {
    logger::trace("ExecutionContext({})::stop() called", getInstanceName());
    if (info_["state"].stringValue() != "stopped") {
        onStopping();
        info_["state"] = Value("stopped");
        onStopped();
        logger::info("ExecutionContext({}) successfully stopped.", getInstanceName());
    }
    return info();
}

void ExecutionContextBase::svc() {
    logger::trace("ExecutionContext::svc()");
    for(auto& op : operations_) {
        try {
            logger::trace("In EC::svc. OperationBase({})::executing....", op->info().at("fullName"));
            auto v = op->execute();
            logger::trace("In EC::svc. execution result is {}", v);
        } catch (std::exception& ex) {
            logger::error("In EC::svc. Exception in ExecutionContext::svc(): {}", ex.what());
        }
    }
}

Value ExecutionContextBase::bind(const std::shared_ptr<OperationAPI>& op) {
    if (op->isNull()) {
        return Value::error(logger::error("ExecutionContext::bind failed. Operation is null"));
    }
    auto info = op->info();     
    for(auto it = operations_.begin(); it != operations_.end();++it) {
        if ((*it)->info().at("fullName") == info.at("fullName")) {
            it = operations_.erase(it);
        }
    }
    operations_.push_back(op);
    return op->info();
}

Value ExecutionContextBase::unbind(const std::string& opName) {
    for(auto it = operations_.begin(); it != operations_.end();++it) {
        if ((*it)->info().at("fullName").stringValue() == opName) {
            it = operations_.erase(it);
            return opName;
        } 
    }
    return Value::error(logger::error("ExecutionContext::unbind({}) failed. Not Found.", opName));
}

std::shared_ptr<OperationAPI> ExecutionContextBase::operation(const std::string& fullName) const { 
    auto op = nerikiri::functional::find<std::shared_ptr<OperationAPI>>(operations(), [&fullName](auto op) { return op->fullName() == fullName; });
    if (op) return op.value();
    return nullOperation();
}

    
