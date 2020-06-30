#pragma once

#include <string>
#include <memory>
#include "nerikiri/value.h"
#include "nerikiri/container.h"
#include "nerikiri/containeroperation.h"
#include "nerikiri/containeroperationfactorybase.h"
namespace nerikiri {

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
            return demangle(typeid(T).name()) + ":" + info_.at("typeName").stringValue();
        }
    public:
        virtual std::shared_ptr<ContainerOperationBase> create(const Value& info) { 
            auto i = nerikiri::merge(info, info_);
            return std::dynamic_pointer_cast<ContainerOperationBase>(std::make_shared<ContainerOperation<T>>(i, function_)); 
        }
    };

    template<typename T>
    void* containerOperationFactory(const Value& info, std::function<Value(T&,Value&&)>&& func) { return new ContainerOperationFactory<T>(info, std::move(func)); }
}