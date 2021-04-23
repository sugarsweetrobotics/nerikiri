#pragma once

#include <string>

#include <nerikiri/value.h>
#include <nerikiri/object.h>

namespace nerikiri {

   

    class FactoryBrokerAPI {
    public:
        virtual ~FactoryBrokerAPI() {}

    public:
        virtual Value createObject(const std::string& className, const Value& info={}) = 0;

        virtual Value deleteObject(const std::string& className, const std::string& fullName) = 0;
    };


    class StoreBrokerAPI {
    public:
        virtual ~StoreBrokerAPI() {}

        virtual Value getObjectInfo(const std::string& className, const std::string& fullName) const = 0;

        virtual Value getClassObjectInfos(const std::string& className) const = 0;

        virtual Value getChildrenClassObjectInfos(const std::string& parentName, const std::string& className) const = 0;
    };


    class OperationBrokerAPI {
    public:
        OperationBrokerAPI()  {}
        virtual ~OperationBrokerAPI() {}

        virtual Value fullInfo(const std::string& fullName) const = 0;

        virtual Value call(const std::string& fullName, const Value& value) = 0;

        virtual Value invoke(const std::string& fullName) = 0;

        virtual Value execute(const std::string& fullName) = 0;
        // collect informations of inlets ex., names.
        virtual Value inlets(const std::string& fullName) const = 0;
    };

    class OperationOutletBrokerAPI {
    public:
        virtual ~OperationOutletBrokerAPI() {}

        //virtual Value ownerFullName() const  = 0;

        virtual Value get(const std::string& fullName) const = 0;

        virtual Value connections(const std::string& fullName) const = 0;

        virtual Value addConnection(const std::string& fullName, const Value& c) = 0;
        
        virtual Value removeConnection(const std::string& fullName, const std::string& name) = 0;

        virtual Value info(const std::string& fullName) const = 0; 

        virtual Value connectTo(const std::string& fullName, const Value& conInfo) = 0;

        virtual Value disconnectFrom(const std::string& fullName, const Value& inletInfo) = 0;
    };

    class OperationInletBrokerAPI {
    public:
        virtual ~OperationInletBrokerAPI() {}

        virtual Value name(const std::string& fullName, const std::string& targetName) const = 0;
        
        virtual Value defaultValue(const std::string& fullName, const std::string& targetName) const = 0;

        virtual Value put(const std::string& fullName, const std::string& targetName, const Value& value) const = 0;

        virtual Value get(const std::string& fullName, const std::string& targetName) const = 0;

        virtual Value info(const std::string& fullName, const std::string& targetName) const = 0; 

        virtual Value isUpdated(const std::string& fullName, const std::string& targetName) const = 0;

        virtual Value connections(const std::string& fullName, const std::string& targetName) const = 0;

        virtual Value addConnection(const std::string& fullName, const std::string& targetName, const Value& c) = 0;
        
        virtual Value removeConnection(const std::string& fullName, const std::string& targetName, const std::string& name) = 0;

        virtual Value connectTo(const std::string& fullName, const std::string& targetName, const Value& conInfo) = 0;

        virtual Value disconnectFrom(const std::string& fullName, const std::string& targetName, const Value& outletInfo) = 0;
    };


    class ConnectionBrokerAPI {
    public:
        virtual ~ConnectionBrokerAPI() {}
    public:
        virtual Value createConnection(const Value& connectionInfo) = 0;
        virtual Value deleteConnection(const std::string& fullName) = 0;
   };


    class ContainerBrokerAPI {  
    public:
        virtual ~ContainerBrokerAPI() {}

        virtual Value operations(const std::string& containerFullName)  const = 0;
    };

    class ContainerOperationBrokerAPI {
    public:
        virtual ~ContainerOperationBrokerAPI() {}
    };




    class ECBrokerAPI {
    public:
        virtual ~ECBrokerAPI() {}

        virtual Value activateStart(const std::string& fullName) const = 0;

        virtual Value activateStop(const std::string& fullName) = 0;
        /*
        virtual Value getECBoundOperations(const std::string& fullName) const = 0;

        virtual Value bindOperationToEC(const std::string& ecFullName, const Value& objectInfo) = 0;

        virtual Value unbindOperationFromEC(const std::string& ecFullName, const Value& objectInfo) = 0;
        */
    };

/*
    class FSMBrokerAPI {
    public:
        virtual ~FSMBrokerAPI() {}

    public:
        virtual Value currentFSMState(const std::string& fsmFullName) = 0;

        virtual Value setFSMState(const std::string& fsmFullName, const std::string& stateFullName) = 0;

        virtual Value fsmStates(const std::string& fsmFullName) = 0;

        virtual Value fsmState(const std::string& fsmFullName, const std::string& stateName) = 0;
    };

    class FSMStateBrokerAPI {
    public:
        virtual ~FSMStateBrokerAPI() {}

    public:
        virtual Value isActive(const std::string& fsmName, const std::string& stateName) = 0;

        virtual Value activate(const std::string& fsmName, const std::string& stateName) = 0;

        virtual Value deactivate(const std::string& fsmName, const std::string& stateName) = 0;

        virtual Value isTransitable(const std::string& fsmName, const std::string& stateName,const std::string& targetStateName) const = 0;

        virtual Value bindOperation(const std::string& fsmName, const std::string& stateName, const Value& info) = 0;

        virtual Value bindOperation(const std::string& fsmName, const std::string& stateName, const Value& info, const Value& arg) = 0;

        virtual Value bindECState(const std::string& fsmName, const std::string& stateName, const Value& info) = 0;

        virtual Value unbindOperation(const std::string& fsmName, const std::string& stateName, const Value& info) = 0;

        virtual Value unbindECState(const std::string& fsmName, const std::string& stateName, const Value& info) = 0;

        virtual Value boundOperations(const std::string& fsmName, const std::string& stateName) = 0;

        virtual Value boundECStates(const std::string& fsmName, const std::string& stateName) = 0;

        virtual Value inlet(const std::string& fsmName, const std::string& stateName) = 0;
    };
*/
    class BrokerProxyAPI : public Object
    {
    private:

        const std::shared_ptr<StoreBrokerAPI> store_;
        const std::shared_ptr<FactoryBrokerAPI> factory_;
        const std::shared_ptr<OperationBrokerAPI> operation_;
        const std::shared_ptr<OperationOutletBrokerAPI> operationOutlet_;
        const std::shared_ptr<OperationInletBrokerAPI> operationInlet_;
        const std::shared_ptr<ConnectionBrokerAPI> connection_;
        const std::shared_ptr<ContainerBrokerAPI> container_;
        //const std::shared_ptr<FSMBrokerAPI> fsm_;
        //const std::shared_ptr<FSMStateBrokerAPI> fsmState_;
        const std::shared_ptr<OperationInletBrokerAPI> fsmStateInlet_;
        const std::shared_ptr<ECBrokerAPI> ec_;

    public:


        virtual std::shared_ptr<FactoryBrokerAPI> factory() { return factory_; }
        virtual std::shared_ptr<const FactoryBrokerAPI> factory() const { return factory_; }
        virtual std::shared_ptr<StoreBrokerAPI>   store() { return store_; }
        virtual std::shared_ptr<const StoreBrokerAPI>   store() const { return store_; }
        virtual std::shared_ptr<OperationBrokerAPI>   operation() { return operation_; }
        virtual std::shared_ptr<const OperationBrokerAPI>   operation() const { return operation_; }
        virtual std::shared_ptr<OperationOutletBrokerAPI>   operationOutlet() { return operationOutlet_; }
        virtual std::shared_ptr<const OperationOutletBrokerAPI>   operationOutlet() const { return operationOutlet_; }
        virtual std::shared_ptr<OperationInletBrokerAPI>   operationInlet() { return operationInlet_; }
        virtual std::shared_ptr<const OperationInletBrokerAPI>   operationInlet() const { return operationInlet_; }
        virtual std::shared_ptr<ConnectionBrokerAPI> connection() { return connection_; }
        virtual std::shared_ptr<const ConnectionBrokerAPI>  connection() const { return connection_; }
        virtual std::shared_ptr<ContainerBrokerAPI> container() { return container_; }
        virtual std::shared_ptr<const ContainerBrokerAPI>  container() const { return container_; }
        //virtual std::shared_ptr<FSMBrokerAPI> fsm() { return fsm_; }
        //virtual std::shared_ptr<const FSMBrokerAPI> fsm() const { return fsm_; }
        //virtual std::shared_ptr<FSMStateBrokerAPI> fsmState() { return fsmState_; }
        //virtual std::shared_ptr<const FSMStateBrokerAPI> fsmState() const { return fsmState_; }
        //virtual std::shared_ptr<OperationInletBrokerAPI> fsmStateInlet() { return fsmStateInlet_; }
        //virtual std::shared_ptr<const OperationInletBrokerAPI> fsmStateInlet() const { return fsmStateInlet_; }
        virtual std::shared_ptr<ECBrokerAPI> ec() { return ec_; }
        virtual std::shared_ptr<const ECBrokerAPI> ec() const { return ec_; }
        
    public:
        BrokerProxyAPI(const std::string& className, const std::string& typeName, const std::string& fullName, 
            const std::shared_ptr<StoreBrokerAPI>& store, 
            const std::shared_ptr<FactoryBrokerAPI>& factory, 
            const std::shared_ptr<OperationBrokerAPI>& operation, 
            const std::shared_ptr<OperationOutletBrokerAPI>& operationOutlet, 
            const std::shared_ptr<OperationInletBrokerAPI>& operationInlet, 
            const std::shared_ptr<ConnectionBrokerAPI>& connection,
            const std::shared_ptr<ContainerBrokerAPI>& container,
            const std::shared_ptr<ECBrokerAPI>& ec
            //const std::shared_ptr<FSMBrokerAPI>& fsm,
            //const std::shared_ptr<FSMStateBrokerAPI>& fsmState,
            //const std::shared_ptr<OperationInletBrokerAPI>& fsmStateInlet
        ): Object(className, typeName, fullName), store_(store), factory_(factory), operation_(operation), operationOutlet_(operationOutlet),
           operationInlet_(operationInlet), connection_(connection), container_(container), ec_(ec)
           //fsm_(fsm), fsmState_(fsmState), fsmStateInlet_(fsmStateInlet) 
        {}

        BrokerProxyAPI(const Value& info) : Object(info) {}

        virtual ~BrokerProxyAPI() {}
    public:

        virtual Value getProcessInfo() const = 0;

        virtual Value getProcessFullInfo() const = 0;
    };

    std::shared_ptr<BrokerProxyAPI> nullBrokerProxy();


}