#pragma once

#include <string>
#include "nerikiri/value.h"
#include "nerikiri/process_store.h"

namespace nerikiri {

    class Path {
    public:
        std::string path;
    };

    class NK_API ObjectMapper {
    private:


    public:
        static Value createResource(Process* proc, const std::string& path, const Value& value, BrokerAPI* receiverBroker = nullptr);
        static Value readResource(ProcessStore* store, const std::string& path);
        static Value updateResource(Process* proc, const std::string& path, const Value& value, BrokerAPI* receiverBroker = nullptr);
        static Value deleteResource(Process* proc, const std::string& path, BrokerAPI* receiverBroker = nullptr);
    };
}