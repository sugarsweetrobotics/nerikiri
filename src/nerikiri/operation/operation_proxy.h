#pragma once
#include <memory>
#include <string>


namespace nerikiri {
    class OperationInletAPI;
    class OperationAPI;
    class ClientProxyAPI;

    std::shared_ptr<OperationOutletAPI> operationOutletProxy(OperationAPI* owner, const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName);

    std::shared_ptr<OperationInletAPI> operationInletProxy(OperationAPI* owner, const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName, const std::string& name);
}