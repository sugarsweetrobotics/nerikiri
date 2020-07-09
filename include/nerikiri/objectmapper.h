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
        static Value createResource(CoreBroker* coreBroker, const std::string& path, const Value& value, BrokerAPI* receiverBroker = nullptr);
        static Value readResource(const CoreBroker* coreBroker, const std::string& path);
        static Value updateResource(CoreBroker* coreBroker, const std::string& path, const Value& value, BrokerAPI* receiverBroker = nullptr);
        static Value deleteResource(CoreBroker* coreBroker, const std::string& path, BrokerAPI* receiverBroker = nullptr);
    };
}