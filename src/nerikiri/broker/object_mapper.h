#pragma once

#include <string>
#include "nerikiri/value.h"
#include "nerikiri/process_store.h"
#include "nerikiri/core_broker.h"
namespace nerikiri {

    class Path {
    public:
        std::string path;
    };

    class NK_API ObjectMapper {
    private:

    public:
        static Value createResource(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& path, const Value& value, const Value& params={}, BrokerAPI* receiverBroker = nullptr);
        static Value readResource(const std::shared_ptr<const ClientProxyAPI>& broker, const std::string& path, const Value& params={}, const Value& receiverBrokerInfo=Value::error(""));
        static Value updateResource(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& path, const Value& value, const Value& params={}, BrokerAPI* receiverBroker = nullptr);
        static Value deleteResource(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& path, const Value& params={}, BrokerAPI* receiverBroker = nullptr);
    };
}