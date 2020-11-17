#include <nerikiri/operation_proxy.h>



using namespace nerikiri;
/// -------------- Operation Outlet

OperationOutletProxy::OperationOutletProxy(OperationAPI* owner, const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName) : owner_(owner), broker_(broker), fullName_(fullName) {}
OperationOutletProxy::~OperationOutletProxy() {}

Value OperationOutletProxy::get() const {
    return broker_->operationOutlet()->get(fullName_);
}

std::vector<std::shared_ptr<ConnectionAPI>> OperationOutletProxy::connections() const {
    auto infos = broker_->operationOutlet()->connections(fullName_);
    if (infos.isError()) return {};

    
}

Value OperationOutletProxy::addConnection(const std::shared_ptr<ConnectionAPI>& c) {
    return broker_->operationOutlet()->addConnection(fullName_, c);
}

Value OperationOutletProxy::removeConnection(const std::string& _fullName){
    return broker_->operationOutlet()->removeConnection(fullName_, _fullName);
}

Value OperationOutletProxy::info() const {
    return broker_->operationOutlet()->info(fullName_);
}



///-------------- Operation Inlet
OperationInletProxy::OperationInletProxy(OperationAPI* owner, const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName, const std::string& name) : owner_(owner), broker_(broker), fullName_(fullName), name_(name) {}
OperationInletProxy::~OperationInletProxy() {}

Value OperationInletProxy::defaultValue() const {
    return broker_->operationInlet()->defaultValue(fullName_, name_);
}

Value OperationInletProxy::get() const {
    return broker_->operationInlet()->get(fullName_, name_);
}

Value OperationInletProxy::put(const Value& value) {
    return broker_->operationInlet()->put(fullName_, name_);
}



Value OperationInletProxy::info() const {
    return broker_->operationInlet()->defaultValue(fullName_, name_);
}

bool OperationInletProxy::isUpdated() const {
    return broker_->operationInlet()->defaultValue(fullName_, name_);
}

std::vector<std::shared_ptr<ConnectionAPI>> OperationInletProxy::connections() const {
    return broker_->operationInlet()->defaultValue(fullName_, name_);
}

Value OperationInletProxy::addConnection(const std::shared_ptr<ConnectionAPI>& c) {
    return broker_->operationInlet()->defaultValue(fullName_, name_);
}

Value OperationInletProxy::removeConnection(const std::string& _fullName) {
    return broker_->operationInlet()->defaultValue(fullName_, name_);
}