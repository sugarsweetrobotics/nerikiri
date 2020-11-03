#pragma once

#include <nerikiri/broker_proxy_api.h>

namespace nerikiri {

    class CRUDBrokerAPI {
    public:
        virtual ~CRUDBrokerAPI () {}

    public:

        virtual Value createObject(const std::string& fullpath, const std::string& info) = 0;

        virtual Value deleteObject(const std::string& fullName) = 0;

        virtual Value updateObject(const std::string& fullName, const Value& info) = 0;

        virtual Value readObject(const std::string& fullName) = 0;
    };

    class CRUDFactoryBroker : public FactoryBrokerAPI {
    private:
        const std::shared_ptr<CRUDBrokerAPI> crudBroker_;
    public:
        CRUDFactoryBroker(const std::shared_ptr<CRUDBrokerAPI>& crudBroker) : FactoryBrokerAPI(), crudBroker_(crudBroker) {}
        virtual ~CRUDFactoryBrokerAPI() {}

    public:
        virtual Value createObject(const std::string& className, const Value& info={}) override {
            return crudBroker_->createObject("/process/" + className + "s/", info);
        }

        virtual Value deleteObject(const std::string& className, const std::string& fullName) override {
            return crudBroker_->deleteObject("/process/" + className + "s/" + fullName);
        }
    };


}