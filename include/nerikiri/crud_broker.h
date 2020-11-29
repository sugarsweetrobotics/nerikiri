#pragma once

#include <nerikiri/json.h>
#include <nerikiri/process_api.h>
#include <nerikiri/broker_api.h>

namespace nerikiri {



    class CRUDBrokerBase : public BrokerAPI {
    private:
    public:
        CRUDBrokerBase(const std::string& typeName, const std::string& fullName) : BrokerAPI(typeName, fullName) {}
        virtual ~CRUDBrokerBase() {}

    public:

        virtual bool run(ProcessAPI* process) override;
        
        virtual void shutdown(ProcessAPI* process) override;

        virtual bool isRunning() const override;

        Value onCreate(ProcessAPI* process, const std::string& fullPath, const Value& value) ;
        Value onRead(ProcessAPI* process, const std::string& fullPath) ;
        Value onUpdate(ProcessAPI* process, const std::string& fullPath, const Value& value) ;
        Value onDelete(ProcessAPI* process, const std::string& fullPath) ;
    };


}