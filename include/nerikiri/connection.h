#pragma once

#include <memory>
#include <vector>
#include <map>
#include <functional>

#include <nerikiri/connection_api.h>
#include <nerikiri/broker_api.h>

namespace nerikiri {

    class InvalidBrokerException : public std::exception {};

    

    std::shared_ptr<ConnectionAPI> createConnection(const std::string& name, const ConnectionAPI::ConnectionType& type, const std::shared_ptr<OperationInletAPI>& inlet, const std::shared_ptr<OperationOutletAPI>& outlet);
}
