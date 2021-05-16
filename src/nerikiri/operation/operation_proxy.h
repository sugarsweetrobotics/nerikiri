#pragma once
#include <memory>
#include <string>


namespace nerikiri {
    class InletAPI;
    class OperationAPI;
    class ClientProxyAPI;


    std::shared_ptr<OperationAPI> operationProxy(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName);

    std::shared_ptr<OutletAPI> operationOutletProxy(OperationAPI* owner, const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName);

    std::shared_ptr<InletAPI> operationInletProxy(OperationAPI* owner, const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName, const std::string& name);
}