#include "nerikiri/connection.h"



using namespace nerikiri;

Connection::Connection(const ConnectionInfo& info, Broker_ptr providerBroker, Broker_ptr consumerBroker) : info_(info), providerBroker_(providerBroker), consumerBroker_(consumerBroker) {
    if (providerBroker_) {
        auto name = info_.at("provider").at("process").at("name").stringValue();
        pull_func_ = [providerBroker, name]() {
            return providerBroker->invokeOperationByName(name);
        };
    }
}

Connection::~Connection() {}
