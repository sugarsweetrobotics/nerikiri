#pragma once

#include <memory>

#include <juiz/operation_api.h>
#include <juiz/broker_api.h>
#include <juiz/client_proxy_api.h>
#include <juiz/value.h>
#include <juiz/process_store.h>


namespace nerikiri {



    class ObjectFactory {
    public:


        static Value createOperation(ProcessStore& store, const Value& info);

        static Value createContainer(ProcessStore& store, const Value& ci);

        static Value createContainerOperation(ProcessStore& store, const Value& cInfo, const Value& info);

        static Value createBroker(ProcessStore& store, const Value& ci);

        static std::shared_ptr<ClientProxyAPI>  createBrokerProxy(ProcessStore& store, const Value& ci);

        static Value createExecutionContext(ProcessStore& store, const Value& value);

        static Value createTopic(ProcessStore& store, const Value& topicInfo);

        static Value createFSM(ProcessStore& store, const Value& fsmInfo);

    public:


        static Value deleteOperation(ProcessStore& store, const std::string& fullName) ;

        static Value deleteContainer(ProcessStore& store, const std::string& fullName) ;
        
        static Value deleteContainerOperation(ProcessStore& store, const std::string& fullName) ;

        static Value deleteExecutionContext(ProcessStore& store, const std::string& fullName) ;

        // static Value deleteFSM(ProcessStore& store, const std::string& fullName) ;
    };
    
}