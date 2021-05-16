#pragma once

#include <vector>
#include <string>

#include <juiz/value.h>
#include <juiz/process_store.h>

namespace juiz {




    class ModuleLoader {
    public:
        static Value loadOperationFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info);

        static Value loadContainerOperationFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info);

        static Value loadContainerFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info);

        static Value loadExecutionContextFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info);

        static Value loadBrokerFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info);
    };

}