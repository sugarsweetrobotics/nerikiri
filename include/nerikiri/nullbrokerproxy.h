#pragma once

#include "nerikiri/abstractbrokerproxy.h"

namespace nerikiri {

    class NullBrokerProxy : public AbstractBrokerProxy {
    public:

        NullBrokerProxy() : AbstractBrokerProxy({{"typeName", "NullBrokerProxy"}, {"instanceName", "null"}, {"fullName", "null"}}) {}
        virtual ~NullBrokerProxy() {}

    public:
        virtual Value readResource(const std::string& path) const override {
            return Value::error("BrokerProxy::readResource failed. BrokerProxy is NullBrokerProxy.");
        }

        virtual Value createResource(const std::string& path, const Value& value) override {
            return Value::error("BrokerProxy::createResource failed. BrokerProxy is NullBrokerProxy.");
        }

        virtual Value updateResource(const std::string& path, const Value& value) override {
            return Value::error("BrokerProxy::updateResource failed. BrokerProxy is NullBrokerProxy.");
        }

        virtual Value deleteResource(const std::string& path) override {
            return Value::error("BrokerProxy::deleteResource failed. BrokerProxy is NullBrokerProxy.");
        }

        


    };

    inline std::shared_ptr<BrokerAPI> nullBrokerProxy() { return std::make_shared<NullBrokerProxy>(); }
}