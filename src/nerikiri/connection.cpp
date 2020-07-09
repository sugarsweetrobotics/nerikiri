#include "nerikiri/logger.h"

#include "nerikiri/connection.h"


using namespace nerikiri;

Connection::Connection():
  info_(Value::error("Connection is null")), providerBroker_(nullptr), consumerBroker_(nullptr), is_null_(true), is_event_(false) {
}

Connection::Connection(const ConnectionInfo& _info, std::shared_ptr<BrokerAPI> providerBroker, std::shared_ptr<BrokerAPI> consumerBroker) : 
info_(_info), providerBroker_(providerBroker), consumerBroker_(consumerBroker), is_null_(true), is_event_(false) {
    if (info_.isError()) {
        logger::error("Connection::Connection failed. Given ConnectionInfo is Error ({})", info_.getErrorMessage());
        return;
    }

    if (providerBroker_) {
        auto operationName = info_.at("output").at("info").at("fullName").stringValue();
        pull_func_ = [providerBroker, operationName]() {
            return providerBroker->invokeOperation(operationName);
        };
        is_null_ = false;
        if(info_.objectValue().count("type") > 0) {
            if (info_.objectValue().at("type").stringValue() == "event") {
                is_event_ = true;
            }
        }
    }

    if (consumerBroker_) {
        auto operationName = info_.at("input").at("info").at("fullName").stringValue();
        auto argName = info_.at("target").at("name").stringValue();
        auto conName = info_.at("name").stringValue();
        push_func_ = [operationName, argName, conName, consumerBroker](const Value& value) { 
            return consumerBroker->putToArgumentViaConnection(operationName, argName, conName, value); };
        is_null_ = false;
    }
    if (_info.objectValue().count("event") > 0) {
        if (_info.at("type").stringValue() == "event") {
            is_event_ = true;
        }
    }
}


Connection Connection::null;
