#pragma once

#include <string>

#include <nerikiri/process_store.h>
#include <nerikiri/value.h>

namespace nerikiri {



    class ProcessBuilder {
    public:

        static void preloadOperations(ProcessStore& store, const Value& config, const std::string& path);
        static void preloadContainers(ProcessStore& store, const Value& config, const std::string& path);
        static void preloadExecutionContexts(ProcessStore& store, const Value& config, const std::string& path);
        static void preloadFSMs(ProcessStore& store, const Value& config, const std::string& path);

        static void preStartFSMs(ProcessStore& store, const Value& config, const std::string& path);
        static void preStartExecutionContexts(ProcessStore& store, const Value& config, const std::string& path);
        static void preloadConnections(ProcessStore& store, const Value& config, const std::string& path);
        static void preloadBrokers(ProcessStore& store, const Value& config, const std::string& path);
        static void preloadTopics(ProcessStore& store, const Value& config, const std::string& path);
        static void preloadCallbacksOnStarted(ProcessStore& store, const Value& config, const std::string& path);

    };
}