#pragma once

#include <memory>

#include "nerikiri/brokerapi.h"
#include "nerikiri/value.h"
#include "nerikiri/process_store.h"


namespace nerikiri {



    class ObjectFactory {
    public:


        static Value createOperation(ProcessStore& store, const Value& info);

        static Value createContainer(ProcessStore& store, const Value& ci);

        static Value createBroker(ProcessStore& store, const Value& ci);

        static std::shared_ptr<BrokerAPI>  createBrokerProxy(ProcessStore& store, const Value& ci);

        static Value createExecutionContext(ProcessStore& store, const Value& value);

        static Value createTopic(ProcessStore& store, const Value& topicInfo);

        static Value createFSM(ProcessStore& store, const Value& fsmInfo);

    public:


        static Value deleteOperation(ProcessStore& store, const std::string& fullName) ;

        static Value deleteContainer(ProcessStore& store, const std::string& fullName) ;
        
        static Value deleteExecutionContext(ProcessStore& store, const std::string& fullName) ;

        static Value deleteFSM(ProcessStore& store, const std::string& fullName) ;
    };
    
}