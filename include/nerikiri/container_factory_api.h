#pragma once

#include <nerikiri/container_api.h>

namespace nerikiri {


    class ContainerFactoryAPI : public Object {
    public:
      ContainerFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}
      virtual ~ContainerFactoryAPI() {}

      virtual std::shared_ptr<ContainerAPI> create(const std::string& fullName) = 0;

      // virtual ContainerFactoryAPI& addOperationFactory(const std::shared_ptr<ContainerOperationFactoryBase>& cof) = 0;
    };

    class NullContainerFactory : public ContainerFactoryAPI {
    public:
      NullContainerFactory() : ContainerFactoryAPI("NullContainerFactory", "NullContainer", "null") {}
      virtual ~NullContainerFactory() {}

      virtual std::shared_ptr<ContainerAPI> create(const std::string& fullName) {
        return nullContainer();
      }

      //virtual Value addOperationFactory(const std::shared_ptr<ContainerOperationFactoryBase>& cof) {
      //  return Value::error(logger::warn("NullContainerFactory::addOperationFactory() called. ContainerFactory is null."));
     // }
    };


}