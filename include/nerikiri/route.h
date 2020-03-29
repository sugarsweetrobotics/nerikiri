#pragma once

#include "nerikiri/value.h"
#include "nerikiri/process_store.h"

namespace {


    class Router {
    public:
        static Value request(std::shared_ptr<ProcessStore> store, const std::string& path);
    };

}