#pragma once

#include <vector>
#include <memory>
#include <string>
#include "nerikiri/container.h"

namespace nerikiri {



    template<typename T>
    class ContainerFactory : public ContainerFactoryBase {
    private:
        std::string typename_;
    public:
        ContainerFactory(): typename_(demangle(typeid(T).name())) {}
        virtual ~ContainerFactory() {}
    public:
        virtual std::string typeName() override { return typename_; }
    public:
        virtual std::shared_ptr<ContainerBase> create(const Value& info) override { 
          auto i = info;
          i["typeName"] = demangle(typeid(T).name());
          //auto c = std::shared_ptr<ContainerBase>(new Container<T>(this, i)); 
          auto c = std::dynamic_pointer_cast<ContainerBase>(std::make_shared<Container<T>>(this, i));
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