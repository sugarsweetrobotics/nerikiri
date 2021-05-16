#pragma once

#include <string>

#include <nerikiri/value.h>
#include <nerikiri/object.h>

namespace nerikiri {

   
    class ProcessClientAPI {
    public:
        virtual ~ProcessClientAPI() {}

    public:
        
    };

    class FactoryClientAPI {
    public:
        virtual ~FactoryClientAPI() {}

    public:
        virtual Value createObject(const std::string& className, const Value& info={}) = 0;

        virtual Value deleteObject(const std::string& className, const std::string& fullName) = 0;
    };


    class StoreClientAPI {
    public:
        virtual ~StoreClientAPI() {}

        virtual Value getObjectInfo(const std::string& className, const std::string& fullName) const = 0;

        virtual Value getClassObjectInfos(const std::string& className) const = 0;

        virtual Value getChildrenClassObjectInfos(const std::string& parentName, const std::string& className) const = 0;
    };


    class OperationClientAPI {
    public:
        OperationClientAPI()  {}
        virtual ~OperationClientAPI() {}

        virtual Value fullInfo(const std::string& fullName) const = 0;

        virtual Value call(const std::string& fullName, const Value& value) = 0;

        virtual Value invoke(const std::string& fullName) = 0;

        virtual Value execute(const std::string& fullName) = 0;
        // collect informations of inlets ex., names.
        virtual Value inlets(const std::string& fullName) const = 0;
    };

    class OutletClientAPI {
    public:
        virtual ~OutletClientAPI() {}

        //virtual Value ownerFullName() const  = 0;

        virtual Value get(const std::string& fullName) const = 0;

        virtual Value connections(const std::string& fullName) const = 0;

        //virtual Value addConnection(const std::string& fullName, const Value& c) = 0;
        
        //virtual Value removeConnection(const std::string& fullName, const std::string& name) = 0;

        virtual Value info(const std::string& fullName) const = 0; 

        virtual Value connectTo(const std::string& fullName, const Value& conInfo) = 0;

        virtual Value disconnectFrom(const std::string& fullName, const Value& inletInfo) = 0;
    };

    class InletClientAPI {
    public:
        virtual ~InletClientAPI() {}

        virtual Value name(const std::string& fullName, const std::string& targetName) const = 0;
        
        virtual Value defaultValue(const std::string& fullName, const std::string& targetName) const = 0;

        virtual Value put(const std::string& fullName, const std::string& targetName, const Value& value) const = 0;

        virtual Value get(const std::string& fullName, const std::string& targetName) const = 0;

        virtual Value info(const std::string& fullName, const std::string& targetName) const = 0; 

        virtual Value isUpdated(const std::string& fullName, const std::string& targetName) const = 0;

        virtual Value connections(const std::string& fullName, const std::string& targetName) const = 0;

        virtual Value connectTo(const std::string& fullName, const std::string& targetName, const Value& conInfo) = 0;

        virtual Value disconnectFrom(const std::string& fullName, const std::string& targetName, const Value& outletInfo) = 0;
    };


    class ConnectionClientAPI {
    public:
        virtual ~ConnectionClientAPI() {}
    public:
        virtual Value createConnection(const Value& connectionInfo) = 0;
        virtual Value deleteConnection(const std::string& fullName) = 0;
   };


    class ContainerClientAPI {  
    public:
        virtual ~ContainerClientAPI() {}

        virtual Value operations(const std::string& containerFullName)  const = 0;
        virtual Value fullInfo(const std::string& containerFullName) const = 0;
    };


    class ClientProxyAPI : public Object
    {
    private:
        const std::shared_ptr<StoreClientAPI> store_;
        const std::shared_ptr<FactoryClientAPI> factory_;
        const std::shared_ptr<OperationClientAPI> operation_;
        const std::shared_ptr<OutletClientAPI> operationOutlet_;
        const std::shared_ptr<InletClientAPI> operationInlet_;
        const std::shared_ptr<ConnectionClientAPI> connection_;
        const std::shared_ptr<ContainerClientAPI> container_;

    public:


        virtual std::shared_ptr<FactoryClientAPI> factory() { return factory_; }
        virtual std::shared_ptr<const FactoryClientAPI> factory() const { return factory_; }
        virtual std::shared_ptr<StoreClientAPI>   store() { return store_; }
        virtual std::shared_ptr<const StoreClientAPI>   store() const { return store_; }
        virtual std::shared_ptr<OperationClientAPI>   operation() { return operation_; }
        virtual std::shared_ptr<const OperationClientAPI>   operation() const { return operation_; }
        virtual std::shared_ptr<OutletClientAPI>   operationOutlet() { return operationOutlet_; }
        virtual std::shared_ptr<const OutletClientAPI>   operationOutlet() const { return operationOutlet_; }
        virtual std::shared_ptr<InletClientAPI>   operationInlet() { return operationInlet_; }
        virtual std::shared_ptr<const InletClientAPI>   operationInlet() const { return operationInlet_; }
        virtual std::shared_ptr<ConnectionClientAPI> connection() { return connection_; }
        virtual std::shared_ptr<const ConnectionClientAPI>  connection() const { return connection_; }
        virtual std::shared_ptr<ContainerClientAPI> container() { return container_; }
        virtual std::shared_ptr<const ContainerClientAPI>  container() const { return container_; }
        
    public:
        ClientProxyAPI(const std::string& className, const std::string& typeName, const std::string& fullName, 
            const std::shared_ptr<StoreClientAPI>& store, 
            const std::shared_ptr<FactoryClientAPI>& factory, 
            const std::shared_ptr<OperationClientAPI>& operation, 
            const std::shared_ptr<OutletClientAPI>& operationOutlet, 
            const std::shared_ptr<InletClientAPI>& operationInlet, 
            const std::shared_ptr<ConnectionClientAPI>& connection,
            const std::shared_ptr<ContainerClientAPI>& container
        ): Object(className, typeName, fullName), store_(store), factory_(factory), operation_(operation), operationOutlet_(operationOutlet),
           operationInlet_(operationInlet), connection_(connection), container_(container)
        {}

        ClientProxyAPI(const Value& info) : Object(info) {}

        virtual ~ClientProxyAPI() {}
    public:

        /// URLスキーム文字列を返す
        virtual std::string scheme() const = 0;

        /// URLドメイン文字列を返す
        virtual std::string domain() const = 0;

        virtual Value getProcessInfo() const = 0;

        virtual Value getProcessFullInfo() const = 0;
    };

    std::shared_ptr<ClientProxyAPI> nullBrokerProxy();


}