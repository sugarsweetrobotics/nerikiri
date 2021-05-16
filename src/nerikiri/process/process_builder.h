#pragma once

#include <string>

#include <juiz/process_store.h>
#include <juiz/value.h>

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
        static void preloadTopics(ProcessStore& store, const std::shared_ptr<ClientProxyAPI>& broker, const Value& config, const std::string& path);
        static void preloadCallbacksOnStarted(ProcessStore& store, const Value& config, const std::string& path);


        static Value publishTopic(ProcessStore& store, const std::shared_ptr<ClientProxyAPI>& broker, const Value& opInfo, const Value& topicInfo);
        static Value subscribeTopic(ProcessStore& store, const std::shared_ptr<ClientProxyAPI>& broker, const Value& opInfo, const std::string& argName, const Value& topicInfo);
    };
}