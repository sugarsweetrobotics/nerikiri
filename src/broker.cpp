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

Value Broker::makeConnection(const ConnectionInfo& ci) const {
    Broker_ptr& broker = process_->getBrokerByBrokerInfo(ci.at("brokerInfo"));
    return this->registerConnection(broker->registerConnection(ci));
}

Value Broker::registerConnection(const ConnectionInfo& ci) const {
    auto from_name = ci.at("from").at("name").stringValue();
    auto from_op = process_->getOperationByName(from_name);
    if (!from_op.isNull()) {
        if (!from_op.hasConnection(ci)) 
            return from_op.addProviderConnection(Connection(ci));
    }

    auto to_name = ci.at("to").at("name").stringValue();
    auto to_op = process_->getOperationByName(to_name);
    if (!to_op.isNull()) {
        if (!to_op.hasConnection(ci)) {
            return to_op.addConsumerConnection(Connection(ci));
        }
    }

    return Value::error("Operation can not found");
}
