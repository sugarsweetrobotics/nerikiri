#pragma once

#include "nerikiri/value.h"
#include "nerikiri/broker_api.h"
#include "nerikiri/process_store.h"

namespace nerikiri {



    class ConnectionBuilder {
    public:
        static Value createOperationConnection(ProcessStore& store, const Value& connectionInfo, BrokerAPI* receiverBroker=nullptr);

        static Value createOperationToOperationConnection(ProcessStore& store, const Value& connectionInfo, BrokerAPI* receiverBroker=nullptr);

        // static Value createFSMStateToOperationConnection(ProcessStore& store, const Value& connectionInfo, BrokerAPI* receiverBroker=nullptr);

        static Value connect(ProcessStore& store, const std::shared_ptr<OperationOutletAPI>& outlet, const std::shared_ptr<OperationInletAPI>& inlet, const Value& connectionInfo);

        static Value connect(const std::shared_ptr<OperationOutletAPI>& outlet, const std::shared_ptr<OperationInletAPI>& inlet, const ConnectionAPI::ConnectionType& type);
        static Value createInletConnection(ProcessStore* store, const Value& connectionInfo, const std::shared_ptr<BrokerProxyAPI>& receiverBroker/*=nullptr*/);
        static Value createOutletConnection(ProcessStore* store, const Value& connectionInfo, const std::shared_ptr<BrokerProxyAPI>& receiverBroker/*=nullptr*/);


        //static Value createConnection(ProcessStore& store, const Value& connectionInfo, BrokerAPI* receiverBroker=nullptr);
        static Value deleteConnection(ProcessStore* store, const std::string& connectionFullName, BrokerAPI* receiverBroker=nullptr);


        //static Value createStateBind(ProcessStore& store, const Value& connectionInfo, BrokerAPI* receiverBroker=nullptr);
        //static Value deleteStateBindI(ProcessStore* store, const Value& connectionInfo, BrokerAPI* receiverBroker=nullptr);

        static Value registerTopicPublisher(ProcessStore& store, const Value& containerInfo, const Value& opInfo, const Value& topicInfo);
        static Value registerTopicPublisher(ProcessStore& store, const Value& opInfo, const Value& topicInfo);

        static Value registerConsumerConnection(ProcessStore* store, const Value& ci);

        static Value registerProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker=nullptr);
    
        static Value deleteConsumerConnection(ProcessStore* store, const std::string& fullName, const std::string& targetArgName, const std::string& conName);
        static Value deleteProviderConnection(ProcessStore* store, const std::string& fullName, const std::string& conName);

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
        static Value _validateOutputConnectionInfo(std::shared_ptr<OperationAPI> op, const Value& conInfo);
        static Value _validateInputConnectionInfo(std::shared_ptr<OperationAPI> op, const Value& conInfo);
       // static Value _validateConnectionInfo(std::shared_ptr<FSMAPI> fsm, const Value& conInfo);
       // static Value _validateInputConnectionInfo(std::shared_ptr<FSMAPI> fsm, const Value& conInfo);
    };
}
