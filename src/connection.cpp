#include "nerikiri/connection.h"



using namespace nerikiri;

Connection::Connection(const ConnectionInfo& _info, Broker_ptr providerBroker, Broker_ptr consumerBroker) : info_(_info), providerBroker_(providerBroker), consumerBroker_(consumerBroker) {
    if (providerBroker_) {
        auto name = info_.at("output").at("info").at("name").stringValue();
        pull_func_ = [providerBroker, name]() {
            return providerBroker->invokeOperationByName(name);
        };
    }

    if (consumerBroker_) {
        auto name = info_.at("output").at("info").at("name").stringValue();
        push_func_ = [this](Value&& value) { return consumerBroker_->pushViaConnection(info_, std::move(value)); };
    }
}

Connection::~Connection() {}
