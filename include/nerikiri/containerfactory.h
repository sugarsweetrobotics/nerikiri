#pragma once

#include <vector>
#include <memory>
#include <string>

namespace nerikiri {


    class ContainerFactoryBase {
    private:
      std::vector<std::shared_ptr<ContainerOperationFactoryBase>> operationFactories_;
    public:
      virtual ~ContainerFactoryBase() {}
    public:
      virtual std::string typeName() = 0;
    public:
      virtual std::shared_ptr<ContainerBase> create(const Value& info) = 0;
      virtual ContainerFactoryBase& addOperationFactory(std::shared_ptr<ContainerOperationFactoryBase> cof) { 
          operationFactories_.push_back(cof);
          return *this;
      }

      friend class ContainerBase;
    };

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

            i.at("operations").object_for_each([&c, this](auto& key, auto& value) {
              auto ii = value;
              ii["name"] = key;
              auto ret = c->createContainerOperation(ii);
              if (ret.isError()) {
                logger::error("ContainerFactory({}) failed. Can not create ContainerOperation({})", typeName(), str(ii));
              }
            });

          }

          return c;
        }
    };

    template<typename T>
    void* containerFactory() { return new ContainerFactory<T>(); }

}