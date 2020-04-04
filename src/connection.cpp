#include "nerikiri/connection.h"



using namespace nerikiri;

Connection::Connection():
  info_(Value::error("Connection is null")), providerBroker_(nullptr), consumerBroker_(nullptr), is_null_(true) {
}

Connection::Connection(const ConnectionInfo& _info, Broker_ptr providerBroker, Broker_ptr consumerBroker) : 
info_(_info), providerBroker_(providerBroker), consumerBroker_(consumerBroker), is_null_(true) {
    if (info_.isError()) {
        logger::error("Connection::Connection failed. Given ConnectionInfo is Error ({})", info_.getErrorMessage());
        return;
    }

    if (providerBroker_) {
        auto info = info_.at("output").at("info");
        pull_func_ = [providerBroker, info]() {
            return providerBroker->invokeOperation(info);
        };
        is_null_ = false;
    }

    if (consumerBroker_) {
        auto name = info_.at("output").at("info").at("name").stringValue();
        push_func_ = [this](Value&& value) { return consumerBroker_->pushViaConnection(info_, std::move(value)); };
        is_null_ = false;
    }

    //throw InvalidBrokerException();
}

Connection::~Connection() {}

Connection Connection::null;
