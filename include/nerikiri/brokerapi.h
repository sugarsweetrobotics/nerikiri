#pragma once

#include <string>

#include "nerikiri/value.h"
#include "nerikiri/object.h"

namespace nerikiri {


    class OperationBrokerAPI {
    public:

        virtual Value getOperationInfos() const = 0;

        virtual Value getOperationInfo(const std::string& fullName) const = 0;

        virtual Value invokeOperation(const std::string& fullName) const = 0;

        virtual Value callOperation(const std::string& fullName, Value&& value) = 0;

        virtual Value executeOperation(const std::string& fullName) = 0;

        virtual Value getOperationConnectionInfos(const std::string& fullName) = 0;




        virtual Value getOperationFactoryInfos() const = 0;

        virtual Value createOperation(const Value& value) = 0;

        virtual Value deleteOperation(const std::string& fullName) = 0;
    };

    class ContainerBrokerAPI {  
    public:

        virtual Value getContainerInfos() const = 0;

        virtual Value getContainerInfo(const std::string& fullName) const = 0;

        virtual Value getContainerOperationInfos(const std::string& fullName) const = 0;

        virtual Value createContainer(const Value& value) = 0;

        virtual Value deleteContainer(const std::string& fullName) = 0;

        virtual Value getContainerFactoryInfos() const = 0;

        virtual Value createContainerOperation(const std::string& fullName, const Value& value) = 0;

        virtual Value deleteContainerOperation(const std::string& fullName) = 0;

        virtual Value getContainerOperationInfo(const std::string& fullName) const  = 0;

        virtual Value getContainerOperationConnectionInfos(const std::string& fullName) = 0;

        virtual Value invokeContainerOperation(const std::string& fullName) const  = 0;

        virtual Value callContainerOperation(const std::string& fullName, Value&& arg) = 0;

        virtual Value executeContainerOperation(const std::string& fullName) = 0;
    };

    class ContainerOperationBrokerAPI {
    public:

    };

    class AllOperationBrokerAPI {
    public:

        virtual Value getAllOperationInfos() const = 0;

        virtual Value getAllOperationInfo(const std::string& fullName) const = 0;

        virtual Value invokeAllOperation(const std::string& fullName) const = 0;

        virtual Value callAllOperation(const std::string& fullName, Value&& arg) const = 0;

        virtual Value executeAllOperation(const std::string& fullName) const = 0;

        virtual Value getAllOperationConnectionInfos(const std::string& fullName) const = 0;
    };
    

    class ConnectionBrokerAPI {
    public:
        virtual Value getConnectionInfos() const = 0;

        virtual Value registerConsumerConnection(const Value& ci)  = 0;

        virtual Value registerProviderConnection(const Value& ci)  = 0;

        virtual Value removeProviderConnection(const std::string& operationFullName, const std::string& conName) = 0;

        virtual Value removeConsumerConnection(const std::string& operationFullName, const std::string& targetArgName, const std::string& conName) = 0;

        virtual Value putToArgument(const std::string& fullName, const std::string& argName, const Value& value) = 0;

        virtual Value putToArgumentViaConnection(const std::string& fullName, const std::string& targetArgName, const std::string& conName, const Value& value) = 0;
    };

    class BrokerAPI : public Object, public OperationBrokerAPI, 
                public ContainerBrokerAPI, 
                public ContainerOperationBrokerAPI, 
                public AllOperationBrokerAPI,
                public ConnectionBrokerAPI
    {
    private:

    public:
        BrokerAPI(): Object() {}

        BrokerAPI(const Value& info) : Object(info) {}

        virtual ~BrokerAPI() {}

        virtual Value getBrokerInfo() const = 0; 

        virtual Value getProcessInfo() const = 0;


        virtual Value createResource(const std::string& path, const Value& value) = 0;

        virtual Value readResource(const std::string& path) const = 0;

        virtual Value updateResource(const std::string& path, const Value& value) = 0;

        virtual Value deleteResource(const std::string& path) = 0;




        virtual Value getExecutionContextInfos() = 0;

        virtual Value getExecutionContextFactoryInfos() = 0;

        virtual Value getExecutionContextInfo(const std::string& fullName) = 0;

        virtual Value createExecutionContext(const Value& value) = 0;

        virtual Value deleteExecutionContext(const std::string& fullName) = 0;

        virtual Value getExecutionContextState(const std::string& fullName) = 0;

        virtual Value setExecutionContextState(const std::string& fullName, const std::string& state) = 0;

        virtual Value getExecutionContextBoundOperationInfos(const std::string& fullName) = 0;

        virtual Value getExecutionContextBoundAllOperationInfos(const std::string& fullName) = 0;

        virtual Value bindOperationToExecutionContext(const std::string& ecFullName, const std::string& opFullName, const Value& opInfo) = 0;

        virtual Value unbindOperationFromExecutionContext(const std::string& ecFullName, const std::string& opFullName) = 0;

        virtual Value getBrokerInfos() const = 0;

        virtual Value getCallbacks() const = 0;



        virtual Value getTopicInfos() = 0;

        virtual Value invokeTopic(const std::string& fullName) = 0;

        virtual Value getInvokeConnectionInfos(const std::string& fullName);
        /*
        virtual Value getGenericFSMInfos() = 0;

        virtual Value createGenericFSM(const Value& info) = 0;

        virtual Value deleteGenericFSM(const Value& info) = 0;

        virtual Value getGenericFSMInfo(const Value& fullName) = 0;

        virtual Value getGenericFSMState(const Value& info) = 0;

        virtual Value setGenericFSMState(const Value& info, const std::string& state) = 0;
        */
    };


}