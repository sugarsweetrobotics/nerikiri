#pragma once

#include <nerikiri/connection_api.h>

namespace nerikiri {

    class ConnectionProxy : public ConnectionAPI {
    public:
        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const = 0;

        virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const = 0;

        virtual Value addOperation(const std::shared_ptr<OperationAPI>& operation) = 0;

        virtual Value deleteOperation(const std::string& fullName) = 0;
    };
}