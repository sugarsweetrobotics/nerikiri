#pragma once

#include <string>
#include <juiz/value.h>

namespace juiz {
    class ProcessStore;

    /**
     * 
     */
    bool setupAnchorContainer(ProcessStore& store);

    /**
     * 
     */
    Value createStaticPeriodicAnchor(ProcessStore& store, const std::string& fullName, const Value& ecInfo);

    /**
     * 
     */
    Value createDynamicPeriodicAnchor(ProcessStore& store, const std::string& fullName, const Value& ecInfo);
}