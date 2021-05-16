#pragma once

#include <memory>

#include <juiz/client_proxy_api.h>
#include <juiz/connection_api.h>

namespace juiz {
    
    std::shared_ptr<ConnectionAPI> connectionProxy(const std::shared_ptr<ClientProxyAPI>& broker, const Value& info);
}