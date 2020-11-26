#pragma once

#include <nerikiri/broker_proxy_api.h>
#include <nerikiri/crud_broker_proxy_api.h>

namespace nerikiri {


    class CRUDFactoryBroker : public FactoryBrokerAPI {
    private:
        CRUDBrokerProxyAPI* broker_;
    public:
        virtual ~CRUDFactoryBroker() {}
        CRUDFactoryBroker(CRUDBrokerProxyAPI* broker) : FactoryBrokerAPI(), broker_(broker) {}

    public:
        virtual Value createObject(const std::string& className, const Value& info={}) {
            return broker_->createResource(className + "s/", info);
        }

        virtual Value deleteObject(const std::string& className, const std::string& fullName) {
            return broker_->deleteResource(className + "s/" + fullName);
        }
    };

    class CRUDStoreBroker : public StoreBrokerAPI {
    private:
        CRUDBrokerProxyAPI* broker_;
    public:
        CRUDStoreBroker (CRUDBrokerProxyAPI* broker) : StoreBrokerAPI(), broker_(broker) {}
        virtual ~CRUDStoreBroker() {}

        virtual Value getObjectInfo(const std::string& className, const std::string& fullName) const {
            return broker_->readResource(className + "s/" + fullName);
        }

        virtual Value getClassObjectInfos(const std::string& className) const {
            return broker_->readResource(className + "s/");
        }

        virtual Value getChildrenClassObjectInfos(const std::string& parentName, const std::string& className) const {

        }
    };


    class CRUDOperationBroker : public OperationBrokerAPI {
    private:
        CRUDBrokerProxyAPI* broker_;
    public:
        CRUDOperationBroker (CRUDBrokerProxyAPI* broker) : OperationBrokerAPI(), broker_(broker) {}
        virtual ~CRUDOperationBroker() {}

        
        virtual Value fullInfo(const std::string& fullName) const override {
            return broker_->readResource("operations/" + fullName + "/fullInfo");
        }

        virtual Value call(const std::string& fullName, const Value& value) override {
            return broker_->updateResource("operations/" + fullName + "/call", value);
        }

        virtual Value invoke(const std::string& fullName) override {
            return broker_->updateResource("operations/" + fullName + "/invoke", {});
        }

        virtual Value execute(const std::string& fullName) override {
            return broker_->updateResource("operations/" + fullName + "/execute", {});
        }
        // collect informations of inlets ex., names.
        virtual Value inlets(const std::string& fullName) const override {
            return broker_->readResource("operations/" + fullName + "/inlets");
        }
    };

    class CRUDOperationOutletBroker : public OperationOutletBrokerAPI {
        CRUDBrokerProxyAPI* broker_;
    public:
        CRUDOperationOutletBroker(CRUDBrokerProxyAPI* broker) : broker_(broker) {}
        virtual ~CRUDOperationOutletBroker() {}

        virtual Value get(const std::string& fullName) const override {
            return broker_->readResource("operations/" + fullName + "/outlet");
        }

        virtual Value connections(const std::string& fullName) const override {
            return broker_->readResource("operations/" + fullName + "/outlet/connections");
        }

        virtual Value addConnection(const std::string& fullName, const Value& c) override {
            return broker_->createResource("operations/" + fullName + "/outlet/connections", c);
        }
        
        virtual Value removeConnection(const std::string& fullName, const std::string& name) override {
            return broker_->deleteResource("operations/" + fullName + "/outlet/connections/" + name);
        }

        virtual Value info(const std::string& fullName) const override {
            return broker_->readResource("operations/" + fullName + "/outlet/info");
        }
    };

    class CRUDOperationInletBroker : public OperationInletBrokerAPI {
        CRUDBrokerProxyAPI* broker_;
    public:
        CRUDOperationInletBroker(CRUDBrokerProxyAPI* broker) : broker_(broker) {}
        virtual ~CRUDOperationInletBroker() {}

        virtual Value name(const std::string& fullName, const std::string& targetName) const override {
            return broker_->readResource("operations/" + fullName + "/inlets/" + targetName + "/name");
        }
        
        virtual Value defaultValue(const std::string& fullName, const std::string& targetName) const override {
            return broker_->readResource("operations/" + fullName + "/inlets/" + targetName + "/defaultValue");
        }

        virtual Value put(const std::string& fullName, const std::string& targetName, const Value& value) const override {
            return broker_->updateResource("operations/" + fullName + "/inlets/" + targetName, value);
        }

        virtual Value get(const std::string& fullName, const std::string& targetName) const override {
            return broker_->readResource("operations/" + fullName + "/inlets/" + targetName);
        }

        virtual Value info(const std::string& fullName, const std::string& targetName) const override {
            return broker_->readResource("operations/" + fullName + "/inlets/" + targetName + "/info");
        }

        virtual Value isUpdated(const std::string& fullName, const std::string& targetName) const override {
            return broker_->readResource("operations/" + fullName + "/inlets/" + targetName + "/isUpdated");
        }

        virtual Value connections(const std::string& fullName, const std::string& targetName) const override {
            return broker_->readResource("operations/" + fullName + "/inlets/" + targetName + "/connections");
        }

        virtual Value addConnection(const std::string& fullName, const std::string& targetName, const Value& c) override {
            return broker_->createResource("operations/" + fullName + "/inlets/" + targetName + "/connections", c);
        }
        
        virtual Value removeConnection(const std::string& fullName, const std::string& targetName, const std::string& name) override {
            return broker_->deleteResource("operations/" + fullName + "/inlets/" + targetName + "/connections/" + name);
        }
    };
    class CRUDConnectionBroker : public ConnectionBrokerAPI {
        CRUDBrokerProxyAPI* broker_;
    public:
        CRUDConnectionBroker(CRUDBrokerProxyAPI* broker) : broker_(broker) {}
        virtual ~CRUDConnectionBroker() {}
    public:
        virtual Value createConnection(const Value& connectionInfo) override {
            return broker_->createResource("connections/", connectionInfo);
        }
        virtual Value deleteConnection(const std::string& fullName) override {
            return broker_->readResource("connections/" + fullName);
        }
    };

    class CRUDBrokerProxyBase : public CRUDBrokerProxyAPI, public BrokerProxyAPI {
    private:
        std::shared_ptr<StoreBrokerAPI> store_;
        std::shared_ptr<FactoryBrokerAPI> factory_;
        std::shared_ptr<OperationBrokerAPI> operation_;
        std::shared_ptr<OperationOutletBrokerAPI> operationOutlet_;
        std::shared_ptr<OperationInletBrokerAPI> operationInlet_;
        std::shared_ptr<ConnectionBrokerAPI> connection_;
    public:
        CRUDBrokerProxyBase(const std::string& typeName, const std::string& fullName) : CRUDBrokerProxyAPI(), BrokerProxyAPI(typeName, fullName), 
          store_(std::make_shared<CRUDStoreBroker>(this)),
          factory_(std::make_shared<CRUDFactoryBroker>(this))
          ,operation_(std::make_shared<CRUDOperationBroker>(this))
          ,operationOutlet_(std::make_shared<CRUDOperationOutletBroker>(this))
          ,operationInlet_(std::make_shared<CRUDOperationInletBroker>(this))
          ,connection_(std::make_shared<CRUDConnectionBroker>(this))
        {}

        virtual ~CRUDBrokerProxyBase() {}

    public:

        virtual Value getProcessInfo() const override {
            return readResource("info");            
        }

        virtual Value getProcessFullInfo() const override {
            return readResource("fullInfo");            
        }

        virtual std::shared_ptr<FactoryBrokerAPI> factory() override { return factory_; }
        virtual std::shared_ptr<const FactoryBrokerAPI> factory() const override { return factory_; }
        virtual std::shared_ptr<StoreBrokerAPI>   store() override { return store_; }
        virtual std::shared_ptr<const StoreBrokerAPI>   store() const override { return store_; }
        virtual std::shared_ptr<OperationBrokerAPI>   operation() override { return operation_; }
        virtual std::shared_ptr<const OperationBrokerAPI>   operation() const override { return operation_; }
        virtual std::shared_ptr<OperationOutletBrokerAPI>   operationOutlet() override { return operationOutlet_; }
        virtual std::shared_ptr<const OperationOutletBrokerAPI>   operationOutlet() const override { return operationOutlet_; }
        virtual std::shared_ptr<OperationInletBrokerAPI>   operationInlet() override { return operationInlet_; }
        virtual std::shared_ptr<const OperationInletBrokerAPI>   operationInlet() const override { return operationInlet_; }
        virtual std::shared_ptr<ConnectionBrokerAPI> connection() override { return connection_; }
        virtual std::shared_ptr<const ConnectionBrokerAPI>  connection() const override { return connection_; }
        
    };
}