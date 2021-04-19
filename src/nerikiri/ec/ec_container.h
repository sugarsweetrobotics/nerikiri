#pragma once

#include <string>
#include <nerikiri/value.h>

namespace nerikiri {
    class ProcessStore;

    /**
     * 
     */
    bool setupECContainer(ProcessStore& store);

    /**
     * 
     */
    Value createEC(ProcessStore& store, const std::string& fullName, const Value& ecInfo);
}