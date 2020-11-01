#pragma once

#include <memory>
#include <vector>
#include <map>
#include <functional>

#include <nerikiri/connection_api.h>
#include <nerikiri/brokerapi.h>

namespace nerikiri {

    class InvalidBrokerException : public std::exception {};

    class Connection : public ConnectionAPI {
    private:
        const std::shared_ptr<OperationInletAPI> inlet_;

        const std::shared_ptr<OperationOutletAPI> outlet_;
    public:
        Connection();

        Connection(const std::string& name, const ConnectionType& type, const std::shared_ptr<OperationInletAPI>& inlet, const std::shared_ptr<OperationOutletAPI>& outlet) :
         ConnectionAPI("Connection", name, type), inlet_(inlet), outlet_(outlet) {}
        
        virtual ~Connection() {}

        virtual std::shared_ptr<OperationInletAPI> inlet() const override { return inlet_; }

        virtual std::shared_ptr<OperationOutletAPI> outlet() const override { return outlet_; }

        virtual Value pull() override;

        virtual Value put(const Value& value) override;
    };
}
