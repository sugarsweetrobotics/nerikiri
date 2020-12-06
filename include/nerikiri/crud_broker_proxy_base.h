#pragma once

#include <nerikiri/broker_proxy_api.h>
#include <nerikiri/crud_broker_proxy_api.h>

namespace nerikiri {


    class CRUDBrokerProxyBase : public CRUDBrokerProxyAPI, public BrokerProxyAPI {
    private:
    public:
        CRUDBrokerProxyBase(const std::string& typeName, const std::string& fullName);

        virtual ~CRUDBrokerProxyBase();

    public:

        virtual Value getProcessInfo() const override;

        virtual Value getProcessFullInfo() const override;
    };
}