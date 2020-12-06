#pragma once

#include <memory>

#include <nerikiri/broker_proxy_api.h>
#include <nerikiri/ec_api.h>

namespace nerikiri {

    std::shared_ptr<ECStateAPI> ecStateStartProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName);
    std::shared_ptr<ECStateAPI> ecStateStopProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName);
}