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
            return broker_->createResource("/" + className + "s/", info);
        }

        virtual Value deleteObject(const std::string& className, const std::string& fullName) {
            return broker_->deleteResource("/" + className + "s/" + fullName);
        }
    };

    class CRUDStoreBroker : public StoreBrokerAPI {
    private:
        CRUDBrokerProxyAPI* broker_;
    public:
        CRUDStoreBroker (CRUDBrokerProxyAPI* broker) : StoreBrokerAPI(), broker_(broker) {}
        virtual ~CRUDStoreBroker() {}

        virtual Value getObjectInfo(const std::string& className, const std::string& fullName) const {
            return broker_->readResource("/" + className + "s/" + fullName);
        }

        virtual Value getClassObjectInfos(const std::string& className) const {
            return broker_->readResource("/" + className + "s/");
        }

        virtual Value getChildrenClassObjectInfos(const std::string& parentName, const std::string& className) const {

        }
    };

    class CRUDBrokerProxyBase : public CRUDBrokerProxyAPI, public BrokerProxyAPI {
    private:
        std::shared_ptr<StoreBrokerAPI> store_;
        std::shared_ptr<FactoryBrokerAPI> factory_;
    public:
        CRUDBrokerProxyBase(const std::string& typeName, const std::string& fullName) : CRUDBrokerProxyAPI(), BrokerProxyAPI(typeName, fullName), 
          store_(std::make_shared<CRUDStoreBroker>(this)),
          factory_(std::make_shared<CRUDFactoryBroker>(this)) {}

        virtual ~CRUDBrokerProxyBase() {}

    public:

        virtual Value getProcessInfo() const override {
            return readResource("/info");            
        }

        virtual Value getProcessFullInfo() const override {
            return readResource("/fullInfo");            
        }

        virtual std::shared_ptr<FactoryBrokerAPI> factory() {

        }

        virtual std::shared_ptr<const FactoryBrokerAPI> factory() const {

        }

        virtual std::shared_ptr<StoreBrokerAPI>   store() { return store_; }

        virtual std::shared_ptr<const StoreBrokerAPI>   store() const { return store_; }
        
    };
}