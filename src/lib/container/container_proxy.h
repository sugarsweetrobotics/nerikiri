#pragma once


#include <memory>
#include <string>


namespace juiz {
    class ContainerAPI;
    class ClientProxyAPI;

    std::shared_ptr<ContainerAPI> containerProxy(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName);

}