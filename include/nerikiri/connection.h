#pragma once

#include <memory>
#include <vector>
#include <map>
#include <functional>

#include <nerikiri/process_store.h>
#include <nerikiri/connection_api.h>
#include <nerikiri/broker_api.h>

namespace nerikiri {

    class ProcessStore;


    class InvalidBrokerException : public std::exception {};

    Value connect(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& name, const std::shared_ptr<OperationInletAPI>& inlet, const std::shared_ptr<OperationOutletAPI>& outlet, const Value& options={});

    std::shared_ptr<ConnectionAPI> createConnection(const std::string& name, const ConnectionAPI::ConnectionType& type, const std::shared_ptr<OperationInletAPI>& inlet, const std::shared_ptr<OperationOutletAPI>& outlet);

    std::shared_ptr<ConnectionAPI> createConnection(const std::string& name, const ConnectionAPI::ConnectionType& type, const std::shared_ptr<OperationInletAPI>& inlet, const std::shared_ptr<OperationOutletAPI>& outlet, const std::shared_ptr<Object>& obj);
}
