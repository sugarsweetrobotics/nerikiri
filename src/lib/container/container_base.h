#pragma once

#include <memory>
#include <juiz/container_api.h>

namespace juiz {
    // class ContainerFactoryAPI;
    std::shared_ptr<ContainerAPI> containerBase(const ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName);
}