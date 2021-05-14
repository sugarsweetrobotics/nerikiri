#pragma once

#include <nerikiri/json.h>
#include <nerikiri/process_api.h>
#include <nerikiri/broker_api.h>

namespace nerikiri {



    class CRUDBrokerBase : public BrokerAPI {
    private:
    public:
        CRUDBrokerBase(const std::string& className, const std::string& typeName, const std::string& fullName) : BrokerAPI(className, typeName, fullName) {}
        virtual ~CRUDBrokerBase() {}

    public:

        virtual bool run(const std::shared_ptr<ClientProxyAPI>& coreBroker) override;
        
        virtual void shutdown(const std::shared_ptr<ClientProxyAPI>& coreBroker) override;

        virtual bool isRunning() const override;

        Value onCreate(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullPath, const Value& value, const Value& ancillaryInfo={}) ;
        Value onRead(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullPath, const Value& ancillaryInfo={}, const Value& brokerInfo={}) ;
        Value onUpdate(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullPath, const Value& value, const Value& ancillaryInfo={}) ;
        Value onDelete(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullPath, const Value& ancillaryInfo={}) ;
    };


}