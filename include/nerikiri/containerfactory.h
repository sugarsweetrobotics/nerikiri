#pragma once

#include <vector>
#include <memory>
#include <string>
#include "nerikiri/container.h"

namespace nerikiri {



    template<typename T>
    class ContainerFactory : public ContainerFactoryBase {
    public:
        ContainerFactory() {}
        virtual ~ContainerFactory() {}
    public:
        virtual std::string typeName() override { return demangle(typeid(T).name()); }
    public:
        virtual std::shared_ptr<ContainerBase> create(const Value& info) override { 
          auto i = info;
          i["name"] = demangle(typeid(T).name());
          auto c = std::shared_ptr<ContainerBase>(new Container<T>(this, i)); 
          if (i.objectValue().count("operations") > 0) {

            i.at("operations").list_for_each([&c, this](auto& value) {
              auto ret = c->createContainerOperation(value);
              if (ret.isError()) {
                //logger::error("ContainerFactory({}) failed. Can not create ContainerOperation({})", typeName(), str(ii));
              }
            });

          }

          return c;
        }
    };

    template<typename T>
    void* containerFactory() { return new ContainerFactory<T>(); }

}