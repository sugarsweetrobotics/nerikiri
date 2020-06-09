#pragma once

#include "nerikiri/value.h"
#include "nerikiri/brokerapi.h"
#include "nerikiri/process_store.h"

namespace nerikiri {



    class ConnectionBuilder {
    public:
        
        static Value registerConsumerConnection(ProcessStore* store, const Value& ci);
        static Value registerProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker=nullptr);
    
        static Value deleteConsumerConnection(ProcessStore* store, const Value& ci);
        static Value deleteProviderConnection(ProcessStore* store, const Value& ci);

        static Value registerTopicPublisher(ProcessStore* store, const Value& opInfo, const Value& topicInfo);
        static Value registerTopicSubscriber(ProcessStore* store, const Value& opInfo, const std::string& argName, const Value& topicInfo);

    private:
        static Value _validateConnectionInfo(std::shared_ptr<OperationBase> op, const Value& conInfo);
    };
}