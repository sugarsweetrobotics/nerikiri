#pragma once

#include <vector>
#include <string>
#include <memory>

#include "container_base.h"
#include <juiz/container_api.h>


namespace juiz {

    class ContainerOperationFactoryBase;

    class ContainerFactoryBase : public ContainerFactoryAPI {
    private:
    public:
      ContainerFactoryBase(const std::string& className, const std::string& typeName, const std::string& fullName);
      virtual ~ContainerFactoryBase();
    public:
    };

}