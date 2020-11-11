#pragma once

#include <memory>

#include <nerikiri/broker_factory_api.h>

namespace nerikiri {

    
    class BrokerAPI;
    class Broker;

/*
    class BrokerFactoryBase : public BrokerFactoryAPI {
    private:
    public:
        BrokerFactoryBase(const std::string& typeName, const std::string& fullName) : BrokerFactoryAPI(typeName, fullName) {}
        virtual ~BrokerFactoryBase() {}

    public:
        virtual std::shared_ptr<Broker> create(const Value& param) = 0;

        virtual std::shared_ptr<BrokerAPI> createProxy(const Value& param) = 0;
    };
    */
}