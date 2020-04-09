#pragma once

#include <string>
#include <memory>

namespace nerikiri {


    class ContainerOperationFactoryBase {
    public:
      virtual ~ContainerOperationFactoryBase() {}
    public:
      virtual std::string containerTypeName() = 0;
      virtual std::string typeName() = 0;
    public:
      virtual std::shared_ptr<ContainerOperationBase> create() = 0;
    };

    template<typename T>
    class ContainerOperationFactory : public ContainerOperationFactoryBase {
    private:
        Value info_;
        std::function<Value(T&,Value&&)> function_;
    public:
        ContainerOperationFactory(const Value& info, std::function<Value(T&,Value&&)> func): info_(std::move(info)), function_(func) {}
        ~ContainerOperationFactory() {}

    public:
        virtual std::string containerTypeName() {
            return demangle(typeid(T).name());
        }

        virtual std::string typeName() {
            return demangle(typeid(T).name()) + ":" + info_.at("name").stringValue();
        }
    public:
        virtual std::shared_ptr<ContainerOperationBase> create() { 
            return std::shared_ptr<ContainerOperationBase>(new ContainerOperation<T>(info_, function_)); 
        }
    };

    template<typename T>
    void* containerOperationFactory(const OperationInfo& info, std::function<Value(T&,Value&&)>&& func) { return new ContainerOperationFactory<T>(info, std::move(func)); }
}