#pragma once

#include <string>
#include <nerikiri/operation_api.h>
#include <nerikiri/broker_proxy_api.h>

namespace nerikiri {

    std::shared_ptr<OperationAPI> operationProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName);

}