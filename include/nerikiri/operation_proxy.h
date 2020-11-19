#pragma once

#include <nerikiri/operation_api.h>
#include <nerikiri/broker_proxy_api.h>

#include <nerikiri/operation_inlet_api.h>
#include <nerikiri/operation_outlet_api.h>


namespace nerikiri {
    

    std::shared_ptr<OperationAPI> operationProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName);

}