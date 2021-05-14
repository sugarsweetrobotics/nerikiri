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

    Value connect(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& name, const std::shared_ptr<InletAPI>& inlet, const std::shared_ptr<OutletAPI>& outlet, const Value& options={});

    std::shared_ptr<ConnectionAPI> createConnection(const std::string& name, const ConnectionAPI::ConnectionType& type, const std::shared_ptr<InletAPI>& inlet, const std::shared_ptr<OutletAPI>& outlet);

    std::shared_ptr<ConnectionAPI> createConnection(const std::string& name, const ConnectionAPI::ConnectionType& type, const std::shared_ptr<InletAPI>& inlet, const std::shared_ptr<OutletAPI>& outlet, const std::shared_ptr<Object>& obj);
}
