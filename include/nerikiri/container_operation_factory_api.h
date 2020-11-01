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
    ContainerOperationFactoryAPI(const std::string& typeName, const std::string& fullName): Object(typeName, fullName) {}
    virtual ~ContainerOperationFactoryAPI() {}

    virtual std::shared_ptr<OperationAPI> create(const std::shared_ptr<ContainerAPI>& container, const std::string& _fullName) const = 0;
  };

  class NullContainerOperationFactory : public ContainerOperationFactoryAPI {
  public:
    virtual std::string containerTypeFullName() const override { return "NullContainer"; }
    virtual std::string operationTypeFullName() const override { return "NullOperation"; }
  public:
    NullContainerOperationFactory() : ContainerOperationFactoryAPI("NullOperationFactory", "null") {}
    virtual ~NullContainerOperationFactory() {}

    virtual std::shared_ptr<OperationAPI> create(const std::shared_ptr<ContainerAPI>& container, const std::string& _fullName) const override {
      return std::make_shared<NullOperation>();
    }
  };


}