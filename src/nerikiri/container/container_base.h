#pragma once

#include <memory>
#include <nerikiri/container_api.h>

namespace nerikiri {
    class ContainerFactoryAPI;
    std::shared_ptr<ContainerAPI> containerBase(ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName);
}