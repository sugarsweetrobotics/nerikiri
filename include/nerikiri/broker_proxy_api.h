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

    class NullFactoryBroker : public FactoryBrokerAPI {
    public:
        virtual ~NullFactoryBroker() {}
    public:
        virtual Value createObject(const std::string& className, const Value& info={}) override {
            return Value::error(logger::error("NullFactoryBroker::createObject({}, {}) called. Object is null.", className, info));
        }

        virtual Value deleteObject(const std::string& className, const std::string& fullName) override {
            return Value::error(logger::error("NullFactoryBroker::createObject({}, {}) called. Object is null.", className, fullName));
        }
    };

    class StoreBrokerAPI {
    public:
        virtual ~StoreBrokerAPI() {}

        virtual Value getObjectInfo(const std::string& className, const std::string& fullName) const = 0;

        virtual Value getClassObjectInfos(const std::string& className) const = 0;

        virtual Value getChildrenClassObjectInfos(const std::string& parentName, const std::string& className) const = 0;
    };

    class NullStoreBroker : public StoreBrokerAPI {
    public:
        virtual ~NullStoreBroker() {}

        virtual Value getObjectInfo(const std::string& className, const std::string& fullName) const override {
            return Value::error(logger::error("NullStoreBroker::getObjectInfo({}, {}) called. Object is null.", className, fullName));
        }

        virtual Value getClassObjectInfos(const std::string& className) const override {
            return Value::error(logger::error("NullStoreBroker::getClassObjectInfos({}) called. Object is null.", className));
        }

        virtual Value getChildrenClassObjectInfos(const std::string& parentName, const std::string& className) const override {
            return Value::error(logger::error("NullStoreBroker::getCildrenClassObjectInfos({}, {}) called. Object is null.", parentName, className));
        }
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

    class BrokerProxyAPI : public Object
    {
    private:

    public:
        BrokerProxyAPI(const std::string& typeName, const std::string& fullName): Object(typeName, fullName) {}

        BrokerProxyAPI(const Value& info) : Object(info) {}

        virtual ~BrokerProxyAPI() {}
    public:
        virtual std::shared_ptr<FactoryBrokerAPI> factory() = 0;
        virtual std::shared_ptr<const FactoryBrokerAPI> factory() const = 0;
        virtual std::shared_ptr<StoreBrokerAPI>   store() = 0;
        virtual std::shared_ptr<const StoreBrokerAPI>   store() const = 0;

    };

    class NullBrokerProxy : public BrokerProxyAPI
    {
    private:

    public:
        NullBrokerProxy(): BrokerProxyAPI("NullBrokerProxy", "null") {}
        virtual ~NullBrokerProxy() {}
    public:
        virtual std::shared_ptr<FactoryBrokerAPI> factory() override {
            return std::make_shared<NullFactoryBroker>();
        }

        virtual std::shared_ptr<const FactoryBrokerAPI> factory() const override{
            return std::make_shared<NullFactoryBroker>();
        }

        virtual std::shared_ptr<StoreBrokerAPI> store() override {
            return std::make_shared<NullStoreBroker>();
        }

        virtual std::shared_ptr<const StoreBrokerAPI> store() const override {
            return std::make_shared<NullStoreBroker>();
        }
    };


}