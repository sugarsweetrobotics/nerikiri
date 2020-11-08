

#include "nerikiri/broker_api.h"
#include "nerikiri/broker_factory.h"

#include "nerikiri/nullbroker.h"
#include "nerikiri/nullbrokerproxy.h"

using namespace nerikiri;

/*
BrokerFactory::BrokerFactory() : Object(), typeName_("NullBroker") {}
BrokerFactory::BrokerFactory(const Value& value) : Object(value), typeName_(value.at("typeName").stringValue()) {}

BrokerFactory::~BrokerFactory() {}

const std::string& BrokerFactory::typeName() { return typeName_; }
    
std::shared_ptr<Broker> BrokerFactory::create(const Value& param) {
    return nullBroker();
}

std::shared_ptr<BrokerAPI> BrokerFactory::createProxy(const Value& param) {
    return nullBrokerProxy();
}

std::shared_ptr<BrokerFactory> nerikiri::nullBrokerFactory() { 
    return std::make_shared<BrokerFactory>();
}
*/