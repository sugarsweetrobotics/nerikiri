#pragma once

#include <string>

#include <nerikiri/value.h>
#include <nerikiri/object.h>

namespace nerikiri {

    class CRUDBrokerAPI {
    public:
        virtual ~CRUDBrokerAPI () {}

    public:

        virtual Value createObject(const std::string& typeName, const std::string& objectTypeName, const std::string& info) = 0;

        virtual Value deleteObject(const std::string& fullName) = 0;

        virtual Value updateObject(const std::string& fullName, const Value& info) = 0;

        virtual Value readObject(const std::string& fullName) = 0;
    };

    class FactoryBrokerAPI {
    public:
        virtual ~FactoryBrokerAPI() {}

    public:
        virtual createObject(const std::string& className, const std::string& typeName, const Value& info={}) = 0;

        virtual deleteObject(const sdt::string& fullName) = 0;
    };

    class ProcessStoreBrokerAPI {
    public:
        virtual ~ProcessStoreBrokerAPI() {}

        virtual getClassObjectInfos(const std::string& className) const = 0;

        virtual getObjectInfos(const std::string& className, const std::string& typeName) const = 0;

    };

    class ObjectBrokerAPI {
    public:
        virtual ~ObjectBrokerAPI() {}

    public:
        virtual getInfo(const std::string& fullName) const = 0;

        virtual getInfo(const std::string& className, const std::string& typeName, const std::string& fullName) const = 0;

        virtual getFullInfo(const std::string& className, const std::string& typeName, const std::string& fullName) const = 0;

        virtual getFullInfo(const std::string& fullName) const = 0;
    };

    class OperationBrokerAPI {
    public:
        virtual ~OperationBrokerAPI() {}

        virtual Value invokeOperation(const std::string& fullName) const = 0;

        virtual Value callOperation(const std::string& fullName, const Value& value) = 0;

        virtual Value executeOperation(const std::string& fullName) = 0;
    };

    class OperationOutletBrokerAPI {
    public:
        virtual ~OperationOutletBrokerAPI() {}

        virtual Value get(const std::string& fullName) const = 0;

        virtual Value connections(const std::string& fullName) const = 0;

        virtual Value addConnection(const std::string& fullName, const Value& c) = 0;
        
        virtual Value removeConnection(const std::string& fullName, const std::string& name) = 0;
    };

    class OperationInletBrokerAPI {
    public:
        virtual ~OperationInletBrokerAPI() {}

        virtual Value name(const std::string& fullName, const std::string& targetName) const = 0;
        
        virtual Value defaultValue(const std::string& fullName, const std::string& targetName) const = 0;

        virtual Value put(const std::string& fullName, const std::string& targetName, const Value& value) const = 0;

        virtual Value get(const std::string& fullName, const std::string& targetName) const = 0;

        virtual Value isUpdated(const std::string& fullName, const std::string& targetName) const = 0;

        virtual Value connections(const std::string& fullName, const std::string& targetName) const = 0;

        virtual Value addConnection(const std::string& fullName, const std::string& targetName, const Value& c) = 0;
        
        virtual Value removeConnection(const std::string& fullName, const std::string& targetName, const std::string& name) = 0;
    };

    class ContainerBrokerAPI {  
    public:
        virtual ~ContainerBrokerAPI() {}
    };

    class ContainerOperationBrokerAPI {
    public:
        virtual ~ContainerOperationBrokerAPI() {}
    };


    class ConnectionBrokerAPI {
    public:
        virtual ~ConnectionBrokerAPI() {}
    public:
   };


    class ECBrokerAPI {
    public:
        virtual ~ECBrokerAPI() {}

        virtual Value getECState(const std::string& fullName) const = 0;

        virtual Value setECState(const std::string& fullName, const std::string& state) = 0;

        virtual Value getECBoundOperations(const std::string& fullName) const = 0;

        virtual Value bindOperationToEC(const std::string& ecFullName, const Value& objectInfo) = 0;

        virtual Value unbindOperationFromEC(const std::string& ecFullName, const Value& objectInfo) = 0;
    };


    class FSMBrokerAPI {
    public:
        virtual ~FSMBrokerAPI() {}

    public:
        virtual Value getFSMStates(const std::string& fsmFullName) = 0;

        virtual Value setFSMState(const std::string& fsmFullName, const std::string& stateFullName) = 0;

        virtual Value getFSMState(const std::string& fsmFullName) const = 0;

        virtual Value getFSMBoundObjects(const std::string& fsmFullName) = 0;

        virtual Value bindObjectToFSMState(const std::string& fsmFullName, const std::string& fsmStateFullName, const Value& objectInfo) = 0;

        virtual Value unbindObjectFromFSMState(const std::string& fsmFullName, const std::string& fsmStateFullName, const Value& objectInfo) = 0;

    };

    class BrokerProxyAPI : public Object, public OperationBrokerAPI, 
                public ContainerBrokerAPI, 
                public ContainerOperationBrokerAPI, 
                public AllOperationBrokerAPI,
                public ConnectionBrokerAPI,
                public ECBrokerAPI,
                public FSMBrokerAPI
    {
    private:

    public:
        BrokerProxyAPI(): Object() {}

        BrokerProxyAPI(const Value& info) : Object(info) {}

        virtual ~BrokerAPI() {}

        virtual Value getBrokerInfo() const = 0; 

        virtual Value getProcessInfo() const = 0;


        virtual Value createResource(const std::string& path, const Value& value) = 0;

        virtual Value readResource(const std::string& path) const = 0;

        virtual Value updateResource(const std::string& path, const Value& value) = 0;

        virtual Value deleteResource(const std::string& path) = 0;


        virtual Value getBrokerInfos() const = 0;

        virtual Value getCallbacks() const = 0;


        virtual Value getTopicInfos() const = 0;

        virtual Value invokeTopic(const std::string& fullName) const = 0;

        virtual Value getTopicConnectionInfos(const std::string& fullName) const = 0;
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