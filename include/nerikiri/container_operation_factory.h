#pragma once

#include <string>
#include <memory>

#include <nerikiri/container_operation.h>
#include <nerikiri/container_operation_factory_api.h>
namespace nerikiri { 

  std::shared_ptr<ContainerOperationFactoryAPI> containerOperationFactoryBase(const std::string& className, const std::string& containerTypeFullName, const std::string& operationTypeFullName, 
    const std::string& fullName, const Value& defaultArgs);

    template<typename T>
    class ContainerOperationFactory : public ContainerOperationFactoryAPI {
    private:
        const Value defaultArgs_;
       // const std::shared_ptr<ContainerOperationFactoryAPI> base_;
        const std::function<Value(T&,const Value&)> function_;
    public:
       // virtual std::string containerTypeFullName() const override { return base_->containerTypeFullName(); }
       // virtual std::string operationTypeFullName() const override { return base_->operationTypeFullName(); }


        ContainerOperationFactory(const std::string& operationTypeFullName, const std::string& fullName, const Value& defaultArgs, std::function<Value(T&,const Value&)> func): 
          ContainerOperationFactoryAPI("ContainerOperationFactory", nerikiri::naming::join(nerikiri::demangle(typeid(T).name()), operationTypeFullName), nerikiri::naming::join(nerikiri::demangle(typeid(T).name()), fullName)),
          /*base_(containerOperationFactoryBase("ContainerOperationFactory", nerikiri::demangle(typeid(T).name()), operationTypeFullName, fullName, defaultArgs)), */function_(func) , defaultArgs_(defaultArgs) {}
        virtual ~ContainerOperationFactory() {}
    public:
        virtual std::shared_ptr<OperationAPI> create(const std::shared_ptr<ContainerAPI>& container, const std::string& fullName, const Value& info=Value::error("")) const override { 
            auto defaultArg = defaultArgs_;
            if (!info.isError()) {

                
                if (info.hasKey("defaultArg")) {
                    defaultArg = Value::merge(defaultArg, info["defaultArg"]);
                }
            }
            return std::make_shared<ContainerOperation<T>>(container, typeName(), fullName, defaultArg, function_);
        }
    };

    template<typename T>
    void* containerOperationFactory(const Value& info, const std::function<Value(T&,const Value&)>& func) { 
        return new ContainerOperationFactory<T>(Value::string(info.at("typeName")), Value::string(info.at("typeName")), info.at("defaultArg"), func);
    }
}