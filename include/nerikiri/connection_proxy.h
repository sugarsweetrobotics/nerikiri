#pragma once

#include <memory>

#include <nerikiri/broker_proxy_api.h>
#include <nerikiri/connection_api.h>

namespace nerikiri {
    
    std::shared_ptr<ConnectionAPI> connectionProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const Value& info);
}