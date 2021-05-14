#pragma once

#include <memory>

#include <nerikiri/client_proxy_api.h>
#include <nerikiri/connection_api.h>

namespace nerikiri {
    
    std::shared_ptr<ConnectionAPI> connectionProxy(const std::shared_ptr<ClientProxyAPI>& broker, const Value& info);
}