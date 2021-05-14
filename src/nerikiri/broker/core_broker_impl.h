#pragma once

#include <memory>
#include <nerikiri/client_proxy_api.h>

namespace nerikiri {

    class ProcessAPI;

    class CoreBroker : public ClientProxyAPI {
    protected:
        ProcessAPI* process_;

    public:

    /**
     * 
     */
    CoreBroker(ProcessAPI* process, const std::string& fullName);

    virtual ~CoreBroker();

        virtual Value getProcessInfo() const override;
        virtual Value getProcessFullInfo() const override;
    };
}