#include "nerikiri/broker.h"

#include "nerikiri/process.h"

using namespace nerikiri;

Broker_ptr Broker::null = Broker_ptr(nullptr);

Value Broker::getProcessInfo() const{
    return process_->info();
}

Value Broker::getProcessOperationInfos() const {
    return process_->getOperationInfos();
}

Value Broker::getOperationInfoByName(const std::string& name) const {
    OperationInfo info;
    process_->getOperationInfos().list_for_each([&name, &info](auto& oi) {
        if (oi["name"].stringValue() == name) {
            info = oi;
        }
    });
    return info;
}

Value Broker::callOperationByName(const std::string& name, Value&& value) const {
    return nerikiri::call_operation(process_->getOperationByName(name), std::move(value));
}

Value Broker::invokeOperationByName(const std::string& name) const {
    return process_->invokeOperationByName(name);
}