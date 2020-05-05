#pragma once

#include <vector>
#include <string>

#include "nerikiri/value.h"
#include "nerikiri/process_store.h"

namespace nerikiri {




    class ModuleLoader {
    public:
        static Value loadOperationFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info);

        static Value loadContainerOperationFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info);

        static Value loadContainerFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info);

        static Value loadExecutionContextFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info);

        static Value loadBrokerFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info);
    };

}