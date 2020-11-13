#pragma once

#include <vector>
#include <string>
#include <memory>

#include <nerikiri/container_base.h>
#include <nerikiri/container_factory_api.h>


namespace nerikiri {

    class ContainerOperationFactoryBase;

    class ContainerFactoryBase : public ContainerFactoryAPI {
    private:
    public:
      ContainerFactoryBase(const std::string& className, const std::string& typeName, const std::string& fullName) : ContainerFactoryAPI(className, typeName, fullName) {}
      virtual ~ContainerFactoryBase() {}
    public:

      friend class ContainerBase;
    };

}