#pragma once

#include <nerikiri/object.h>
#include <nerikiri/factory_api.h>
#include <nerikiri/container_api.h>
#include <nerikiri/operation_api.h>

namespace nerikiri {

  class ContainerOperationFactoryAPI : public FactoryAPI {
  public:
    ContainerOperationFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName)
      : FactoryAPI(className, typeName, fullName) {}
    virtual ~ContainerOperationFactoryAPI() {}

    // virtual std::shared_ptr<Object> create(const std::string& _fullName, const Value& info=Value::error("")) const = 0;
  };

  std::shared_ptr<ContainerOperationFactoryAPI> nullContainerOperationFactory();

  template<>
  inline std::shared_ptr<ContainerOperationFactoryAPI> nullObject() { return nullContainerOperationFactory(); }
}