#pragma once

#include <nerikiri/object.h>

namespace nerikri {

    class BrokerFactoryAPI : public Object{
    private:
    public:
        BrokerFactoryAPI(const std::string& typeName, const std::string& fullName) : public Object(typeName, fullName) {}
        virtual ~BrokerFactoryAPI() {}

    public:
        virtual std::shared_ptr<Broker> create(const Value& param) = 0;

        virtual std::shared_ptr<BrokerAPI> createProxy(const Value& param) = 0;
    };

    class NullBrokerFactory : public BrokerFactoryAPI {
    public:
        NullBrokerFactory() : BrokerFactoryAPI("NullBrokerFactory", "null") {}
        virtual ~NullBrokerFactory() {}

    public:

        virtual std::shared_ptr<Broker> create(const Value& param) override {
            logger::error("NullBrokerFactory::{}() failed. Object is null.", __func__, param);
            return std::make_shared<NullBroker>();
        }

        virtual std::shared_ptr<BrokerAPI> createProxy(const Value& param) override {
            logger::error("NullBrokerFactory::{}() failed. Object is null.", __func__, param);
            return std::make_shared<NullBroker>();
        }
    
    };
}