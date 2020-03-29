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
        static Value createResource(ProcessStore* store, const std::string& path, const Value& value);
    };
}