#pragma once

#include <string>
#include <memory>

#include <nerikiri/container_operation.h>
#include <nerikiri/container_operation_factory_base.h>
namespace nerikiri {


    template<typename T>
    class ContainerOperationFactory : public ContainerOperationFactoryBase {
    private:
        std::function<Value(T&,Value&&)> function_;
    public:
        ContainerOperationFactory(const std::string& operationTypeFullName, const std::string& fullName, const Value& defaultArgs, std::function<Value(T&,Value&&)> func):
          ContainerOperationFactoryBase("ContainerOperationFactory", nerikiri::demangle(typeid(T).name()), operationTypeFullName, fullName, defaultArgs), function_(func) {}
        virtual ~ContainerOperationFactory() {}
    public:
        virtual std::shared_ptr<OperationAPI> create(const std::shared_ptr<ContainerAPI>& container, const std::string& fullName) const override { 
            return std::make_shared<ContainerOperation<T>>(container, operationTypeFullName(), fullName, defaultArgs_, function_);
        }
    };

    template<typename T>
    void* containerOperationFactory(const Value& info, std::function<Value(T&,Value&&)>&& func) { return new ContainerOperationFactory<T>(Value::string(info.at("typeName")), Value::string(info.at("fullName")), info.at("defaultArgs"), func); }
}