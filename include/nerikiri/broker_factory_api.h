#pragma once

#include <nerikiri/broker_api.h>
#include <nerikiri/broker_proxy_api.h>

namespace nerikiri {

    class BrokerAPI;

    class BrokerFactoryAPI : public nerikiri::Object {
    private:
    public:
        BrokerFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}
        virtual ~BrokerFactoryAPI() {}

    public:
        virtual std::shared_ptr<BrokerAPI> create(const Value& param) = 0;

        virtual std::shared_ptr<BrokerProxyAPI> createProxy(const Value& param) = 0;
    };


    std::shared_ptr<BrokerFactoryAPI> nullBrokerFactory();
}