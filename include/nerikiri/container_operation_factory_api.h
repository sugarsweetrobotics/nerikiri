#pragma once

#include <nerikiri/object.h>
#include <nerikiri/container_api.h>
#include <nerikiri/operation_api.h>

namespace nerikiri {

  class ContainerOperationFactoryAPI : public Object {
  public:
    virtual std::string containerTypeFullName() const = 0;
    virtual std::string operationTypeFullName() const = 0;
  public:
    ContainerOperationFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName): Object(className, typeName, fullName) {}
    virtual ~ContainerOperationFactoryAPI() {}

    virtual std::shared_ptr<OperationAPI> create(const std::shared_ptr<ContainerAPI>& container, const std::string& _fullName) const = 0;
  };


  std::shared_ptr<ContainerOperationFactoryAPI> nullContainerOperationFactory();
}