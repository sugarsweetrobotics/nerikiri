#pragma once

#include <string>
#include <nerikiri/fsm_api.h>
#include <nerikiri/broker_proxy_api.h>

namespace nerikiri {

    std::shared_ptr<FSMAPI> fsmProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName);

}