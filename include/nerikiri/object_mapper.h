#pragma once

#include <string>
#include "nerikiri/value.h"
#include "nerikiri/process_store.h"
#include "nerikiri/corebroker.h"
namespace nerikiri {

    class Path {
    public:
        std::string path;
    };

    class NK_API ObjectMapper {
    private:

    public:
        static Value createResource(BrokerProxyAPI* coreBroker, const std::string& path, const Value& value, const std::map<std::string, std::string>& params={}, BrokerAPI* receiverBroker = nullptr);
        static Value readResource(const BrokerProxyAPI* coreBroker, const std::string& path, const std::map<std::string, std::string>& params={});
        static Value updateResource(BrokerProxyAPI* coreBroker, const std::string& path, const Value& value, const std::map<std::string, std::string>& params={}, BrokerAPI* receiverBroker = nullptr);
        static Value deleteResource(BrokerProxyAPI* coreBroker, const std::string& path, const std::map<std::string, std::string>& params={}, BrokerAPI* receiverBroker = nullptr);
    };
}