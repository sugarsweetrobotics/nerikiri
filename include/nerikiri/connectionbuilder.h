#pragma once

#include "nerikiri/value.h"
#include "nerikiri/brokerapi.h"
#include "nerikiri/process_store.h"

namespace nerikiri {



    class ConnectionBuilder {
    public:

        static Value createConnection(ProcessStore* store, const Value& connectionInfo, BrokerAPI* receiverBroker=nullptr);
        static Value deleteConnection(ProcessStore* store, const Value& connectionInfo, BrokerAPI* receiverBroker=nullptr);

        static Value registerConsumerConnection(ProcessStore* store, const Value& ci);

        static Value registerProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker=nullptr);
    
        static Value deleteConsumerConnection(ProcessStore* store, const std::string& fullName, const std::string& targetArgName, const std::string& conName);
        static Value deleteProviderConnection(ProcessStore* store, const std::string& fullName, const std::string& conName);

        static Value registerTopicPublisher(ProcessStore* store, const Value& opInfo, const Value& topicInfo);
        static Value registerTopicSubscriber(ProcessStore* store, const Value& opInfo, const std::string& argName, const Value& topicInfo);

        static Value registerOperationConsumerConnection(ProcessStore* store, const Value& ci);
        static Value registerOperationProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker=nullptr);
        static Value deleteOperationConsumerConnection(ProcessStore* store, const std::string& fullName, const std::string& targetArgName, const std::string& conName);
        static Value deleteOperationProviderConnection(ProcessStore* store, const std::string& fullName, const std::string& conName);

        static Value registerFSMConsumerConnection(ProcessStore* store, const Value& ci);
        static Value registerFSMProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker=nullptr);

        static Value bindOperationToFSM(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker=nullptr);
        static Value unbindOperationToFSM(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker=nullptr);


    private:
        static Value _validateOutputConnectionInfo(std::shared_ptr<OperationBase> op, const Value& conInfo);
        static Value _validateInputConnectionInfo(std::shared_ptr<OperationBase> op, const Value& conInfo);
        static Value _validateConnectionInfo(std::shared_ptr<FSM> fsm, const Value& conInfo);
        static Value _validateInputConnectionInfo(std::shared_ptr<FSM> fsm, const Value& conInfo);
    };
}