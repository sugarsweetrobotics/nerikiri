#pragma once

#include <memory>
#include <cstdint>

#include <juiz/object.h>
#include <juiz/operation.h>
#include <juiz/ec.h>

#include <juiz/client_proxy_api.h>

#include <juiz/process_store.h>

namespace juiz {
    class ProcessStore;
    
    class ProcessAPI : public Object {

    private:
    
    public:
        ProcessAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}
        virtual ~ProcessAPI() {}


    public:
        virtual int32_t start() = 0;
        virtual void startAsync() = 0;
        virtual int32_t wait() = 0;
        virtual Value fullInfo() const = 0;
        virtual void stop() = 0;    
        virtual bool isRunning() = 0;

        virtual ProcessStore* store() = 0;

        virtual const ProcessStore* store() const = 0;

        virtual ProcessAPI& load(const std::shared_ptr<FactoryAPI>& f) = 0;

        virtual ProcessAPI& loadECFactory(const std::shared_ptr<ExecutionContextFactoryAPI>& ef) = 0; 

        virtual ProcessAPI& conductProcess(const Value& brokerInfo) = 0;
    };

}