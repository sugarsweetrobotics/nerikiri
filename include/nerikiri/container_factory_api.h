#pragma once

#include <nerikiri/container_api.h>

namespace nerikiri {


    class ContainerFactoryAPI : public Object {
    public:
      ContainerFactoryAPI(const std::string& typeName, const std::string& fullName) : Object(typeName, fullName) {}
      virtual ~ContainerFactoryAPI() {}

      virtual std::string containerTypeFullName() const = 0;

      virtual std::shared_ptr<ContainerAPI> create(const std::string& fullName) = 0;

      // virtual ContainerFactoryAPI& addOperationFactory(const std::shared_ptr<ContainerOperationFactoryBase>& cof) = 0;
    };

    class NullContainerFactory : public ContainerFactoryAPI {
    public:
      NullContainerFactory() : ContainerFactoryAPI("NullContainerFactory", "null") {}
      virtual ~NullContainerFactory() {}

      virtual std::string containerTypeFullName() const { return "NullContainer"; }

      virtual std::shared_ptr<ContainerAPI> create(const std::string& fullName) {
        return std::make_shared<NullContainer>();
      }

      //virtual Value addOperationFactory(const std::shared_ptr<ContainerOperationFactoryBase>& cof) {
      //  return Value::error(logger::warn("NullContainerFactory::addOperationFactory() called. ContainerFactory is null."));
     // }
    };


}