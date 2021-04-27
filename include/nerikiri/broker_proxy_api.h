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
        
    public:
        BrokerProxyAPI(const std::string& className, const std::string& typeName, const std::string& fullName, 
            const std::shared_ptr<StoreBrokerAPI>& store, 
            const std::shared_ptr<FactoryBrokerAPI>& factory, 
            const std::shared_ptr<OperationBrokerAPI>& operation, 
            const std::shared_ptr<OperationOutletBrokerAPI>& operationOutlet, 
            const std::shared_ptr<OperationInletBrokerAPI>& operationInlet, 
            const std::shared_ptr<ConnectionBrokerAPI>& connection,
            const std::shared_ptr<ContainerBrokerAPI>& container
        ): Object(className, typeName, fullName), store_(store), factory_(factory), operation_(operation), operationOutlet_(operationOutlet),
           operationInlet_(operationInlet), connection_(connection), container_(container)
        {}

        BrokerProxyAPI(const Value& info) : Object(info) {}

        virtual ~BrokerProxyAPI() {}
    public:

        virtual Value getProcessInfo() const = 0;

        virtual Value getProcessFullInfo() const = 0;
    };

    std::shared_ptr<BrokerProxyAPI> nullBrokerProxy();


}