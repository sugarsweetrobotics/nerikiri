#pragma once

#include <string>
#include "nerikiri/value.h"
#include "nerikiri/process_store.h"

namespace nerikiri {

    class Path {
    public:
        std::string path;
    };

    class ObjectMapper {
    private:


    public:
        static Value requestResource(ProcessStore* store, const std::string& path);
        static Value createResource(Process* store, const std::string& path, const Value& value, BrokerAPI* receiverBroker = nullptr);
    };
}