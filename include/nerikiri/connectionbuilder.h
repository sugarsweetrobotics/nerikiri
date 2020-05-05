#pragma once

#include "nerikiri/value.h"
#include "nerikiri/brokers/brokerapi.h"
#include "nerikiri/process_store.h"

namespace nerikiri {



    class ConnectionBuilder {
    public:
    
    static Value registerConsumerConnection(ProcessStore* store, const Value& ci);
    static Value registerProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker=nullptr);
  
    static Value deleteConsumerConnection(ProcessStore* store, const Value& ci);
    static Value deleteProviderConnection(ProcessStore* store, const Value& ci);

    };
}