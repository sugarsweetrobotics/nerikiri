#pragma once

#include <memory>
#include "nerikiri/value.h"

namespace nerikiri {

    class BrokerAPI;
    class Broker;

    class BrokerFactory : public Object{
    private:
        const std::string typeName_;
    public:
        BrokerFactory();
        BrokerFactory(const Value& value);

        virtual ~BrokerFactory();

    public:
        const std::string& typeName();
      
    public:
        virtual std::shared_ptr<Broker> create(const Value& param);

        virtual std::shared_ptr<BrokerAPI> createProxy(const Value& param);
    };

    std::shared_ptr<BrokerFactory> nullBrokerFactory();
}