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
      const std::string containerTypeName_;
//      std::vector<std::shared_ptr<ContainerOperationFactoryBase>> operationFactories_;
    public:
      ContainerFactoryBase(const std::string& typeName, const std::string& containerTypeName, const std::string& fullName) : ContainerFactoryAPI(typeName, fullName), containerTypeName_(containerTypeName) {}
      virtual ~ContainerFactoryBase() {}
    public:
      virtual std::string containerTypeFullName() const { return containerTypeName_; }

      friend class ContainerBase;
    };

}