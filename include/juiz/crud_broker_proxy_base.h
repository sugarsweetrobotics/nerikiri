#pragma once

#include <juiz/client_proxy_api.h>
#include <juiz/crud_broker_proxy_api.h>

namespace juiz {


    class CRUDBrokerProxyBase : public CRUDBrokerProxyAPI, public ClientProxyAPI {
    private:
    public:
        CRUDBrokerProxyBase(const std::string& className, const std::string& typeName, const std::string& fullName);

        virtual ~CRUDBrokerProxyBase();

    public:

        virtual Value getProcessInfo() const override;

        virtual Value getProcessFullInfo() const override;
    };
}