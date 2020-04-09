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
      virtual std::shared_ptr<ContainerBase> create() = 0;
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
        virtual std::shared_ptr<ContainerBase> create() override { 
          return std::shared_ptr<ContainerBase>(new Container<T>(this, {{"name", demangle(typeid(T).name())}})); 
        }
    };



    template<typename T>
    void* containerFactory() { return new ContainerFactory<T>(); }

}