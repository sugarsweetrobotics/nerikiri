#pragma once

#include <string>
#include <memory>

#include <nerikiri/container_operation.h>
#include <nerikiri/container_operation_factory_api.h>
namespace nerikiri { 

    /**
     * 
     */
    template<typename T>
    class ContainerOperationFactory : public ContainerOperationFactoryAPI {
    private:
        const Value defaultArgs_;
        const std::function<Value(T&,const Value&)> function_;
    public:
        /**
         * コンストラクタ
         * @param typeName: オペレーションのtypeName
         */
        ContainerOperationFactory(const std::string& typeName, const Value& defaultArgs, std::function<Value(T&,const Value&)> func)
          : ContainerOperationFactoryAPI("ContainerOperationFactory", naming::join(nerikiri::demangle(typeid(T).name()), typeName), naming::join(nerikiri::demangle(typeid(T).name()), typeName)), defaultArgs_(defaultArgs), function_(func)
        {}

        /**
         * デストラクタ
         */
        virtual ~ContainerOperationFactory() {}
    public:

        /**
         * 
         */
        virtual std::shared_ptr<Object> create(const std::string& fullName, const Value& info=Value::error("")) const override { 
            auto defaultArg = defaultArgs_;
            if (info.isError()) {
                //defaultArg = info["defaultArg"];
            }
            if (!info.isError() && info.hasKey("defaultArg")) {
                defaultArg = Value::merge(defaultArg, info["defaultArg"]);    
            }
            return std::make_shared<ContainerOperation<T>>(typeName(), fullName, defaultArg, function_);
        }
    };

    /**
     * 
     */
    template<typename T>
    void* containerOperationFactory(const Value& info, const std::function<Value(T&,const Value&)>& func) { 
        return new ContainerOperationFactory<T>(Value::string(info["typeName"]), info["defaultArg"], func);
    }
}
