#pragma once

#include <memory>
#include <cstdint>

#include <nerikiri/object.h>
#include <nerikiri/operation.h>
#include <nerikiri/ec.h>

#include <nerikiri/broker_proxy_api.h>

#include <nerikiri/process_store.h>

namespace nerikiri {
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

        // virtual std::shared_ptr<BrokerProxyAPI> coreBroker() = 0;

        virtual ProcessStore* store() = 0;
        virtual const ProcessStore* store() const = 0;

        virtual ProcessAPI& loadOperationFactory(const std::shared_ptr<OperationFactoryAPI>& opf) = 0;

        virtual ProcessAPI& loadContainerFactory(const std::shared_ptr<ContainerFactoryAPI>& cf) = 0;

        virtual ProcessAPI& loadContainerOperationFactory(const std::shared_ptr<ContainerOperationFactoryAPI>& copf) = 0;

        virtual ProcessAPI& loadECFactory(const std::shared_ptr<ExecutionContextFactoryAPI>& ef) = 0; 
    };

}