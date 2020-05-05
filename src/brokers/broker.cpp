#include "nerikiri/brokers/broker.h"

#include "nerikiri/process.h"
#include "nerikiri/operation.h"
#include "nerikiri/connection.h"
#include "nerikiri/objectmapper.h"

using namespace nerikiri;

std::shared_ptr<Broker> Broker::null = std::shared_ptr<Broker>(new Broker());

std::shared_ptr<BrokerAPI> BrokerAPI::null = std::shared_ptr<BrokerAPI>(new NullBroker());
std::shared_ptr<BrokerFactory> BrokerFactory::null = std::shared_ptr<BrokerFactory>(nullptr);