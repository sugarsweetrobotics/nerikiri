#include "nerikiri/broker.h"

#include "nerikiri/process.h"
#include "nerikiri/operation.h"
#include "nerikiri/connection.h"
#include "nerikiri/objectmapper.h"

using namespace nerikiri;

std::shared_ptr<BrokerAPI> Broker::null = std::shared_ptr<BrokerAPI>(nullptr);
