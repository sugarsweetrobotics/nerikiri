#pragma once

#include <nerikiri/container_api.h>

namespace nerikiri {


    class ContainerFactoryAPI : public Object {
    public:
      ContainerFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}
      virtual ~ContainerFactoryAPI() {}

      virtual std::shared_ptr<ContainerAPI> create(const std::string& fullName) = 0;
    };


    std::shared_ptr<ContainerFactoryAPI> nullContainerFactory() ;

}