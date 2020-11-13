#pragma once

#include <nerikiri/broker_api.h>
#include <nerikiri/broker_proxy_api.h>

namespace nerikiri {

    class BrokerFactoryAPI : public nerikiri::Object {
    private:
    public:
        BrokerFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}
        virtual ~BrokerFactoryAPI() {}

    public:
        //virtual std::string brokerTypeFullName() const = 0;
        virtual std::shared_ptr<BrokerAPI> create(const Value& param) = 0;

        virtual std::shared_ptr<BrokerProxyAPI> createProxy(const Value& param) = 0;
    };

    class NullBrokerFactory : public BrokerFactoryAPI {
    public:
        NullBrokerFactory() : BrokerFactoryAPI("NullBrokerFactory", "NullBroker", "null") {}
        virtual ~NullBrokerFactory() {}

    public:
        //virtual std::string brokerTypeFullName() const override {
        //    return "NullBroker";
        //}

        virtual std::shared_ptr<BrokerAPI> create(const Value& param) override {
            logger::error("NullBrokerFactory::{}() failed. Object is null.", __func__, param);
            return std::make_shared<NullBroker>();
        }

        virtual std::shared_ptr<BrokerProxyAPI> createProxy(const Value& param) override {
            logger::error("NullBrokerFactory::{}() failed. Object is null.", __func__, param);
            return std::make_shared<NullBrokerProxy>();
        }
    
    };
}