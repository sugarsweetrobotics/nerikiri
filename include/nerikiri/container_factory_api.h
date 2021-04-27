#pragma once

#include <nerikiri/container_api.h>
#include <nerikiri/factory_api.h>

namespace nerikiri {


    class ContainerFactoryAPI : public FactoryAPI {
    public:
      ContainerFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : FactoryAPI(className, typeName, fullName) {}
      virtual ~ContainerFactoryAPI() {}

      // virtual std::shared_ptr<ContainerAPI> create(const std::string& fullName, const Value& info={}) = 0;
    };


    std::shared_ptr<ContainerFactoryAPI> nullContainerFactory() ;

    template<>
    inline std::shared_ptr<ContainerFactoryAPI> nullObject() { return nullContainerFactory(); }
}