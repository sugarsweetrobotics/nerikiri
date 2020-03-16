#include "nerikiri/connection.h"



using namespace nerikiri;

Connection::Connection(const ConnectionInfo& info, Broker_ptr providerBroker, Broker_ptr consumerBroker) : info_(info), providerBroker_(providerBroker), consumerBroker_(consumerBroker) {
    if (providerBroker_) {
        auto name = info.at("provider").at("info").at("name").stringValue();
        pull_func_ = [providerBroker, name]() {
            return providerBroker->invokeOperationByName(name);
        };
    }

    if (consumerBroker_) {
        auto name = info_.at("provider").at("info").at("name").stringValue();
        /*
        push_func_ = [info, consumerBroker, name](Value& value) {
            return consumerBroker->pushForConnectionByName(info.at("consumer").at("target").at("name").stringValue(), value);
        };
        */
    }
}

Connection::~Connection() {}
