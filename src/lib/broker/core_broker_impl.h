#pragma once

#include <memory>
#include <juiz/client_proxy_api.h>

namespace juiz {

    class ProcessAPI;

    class CoreBroker : public ClientProxyAPI {
    protected:
        ProcessAPI* process_;

    public:

    /**
     * 
     */
    CoreBroker(ProcessAPI* process);

    virtual ~CoreBroker();

        /// URLスキーム文字列を返す
        virtual std::string scheme() const override {
            return "";
        }

        virtual std::string domain() const override {
            return "";
        }
        
        virtual Value getProcessInfo() const override;
        virtual Value getProcessFullInfo() const override;
    };
}