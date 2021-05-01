#pragma once

#include <memory>
#include <nerikiri/container_api.h>
#include <nerikiri/container_factory_api.h>

namespace nerikiri {
    // class ContainerFactoryAPI;
    std::shared_ptr<ContainerAPI> containerBase(const ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName);
}