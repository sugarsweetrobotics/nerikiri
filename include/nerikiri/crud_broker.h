#pragma once

#include <nerikiri/json.h>
#include <nerikiri/broker_api.h>

namespace nerikiri {

    class Process;


    class CRUDBrokerBase : public BrokerAPI {
    private:
    public:
        CRUDBrokerBase(const std::string& typeName, const std::string& fullName) : BrokerAPI(typeName, fullName) {}
        virtual ~CRUDBrokerBase() {}

    public:

        virtual bool run(Process* process);
        
        virtual void shutdown(Process* process);

        virtual bool isRunning() const;

        Value onCreate(Process* process, const std::string& fullPath, const Value& value) {}
        Value onRead(Process* process, const std::string& fullPath) {}
        Value onUpdate(Process* process, const std::string& fullPath, const Value& value) {}
        Value onDelete(Process* process, const std::string& fullPath) {}
    };

    /*
    class CRUDFactoryBroker : public FactoryBrokerAPI {
    private:
        const std::shared_ptr<CRUDBrokerAPI> crudBroker_;
    public:
        CRUDFactoryBroker(const std::shared_ptr<CRUDBrokerAPI>& crudBroker) : FactoryBrokerAPI(), crudBroker_(crudBroker) {}
        virtual ~CRUDFactoryBroker() {}

    public:
        virtual Value createObject(const std::string& className, const Value& info={}) override {
            return crudBroker_->createObject("/process/" + className + "s/", info);
        }

        virtual Value deleteObject(const std::string& className, const std::string& fullName) override {
            return crudBroker_->deleteObject("/process/" + className + "s/" + fullName);
        }
    };
    */

}