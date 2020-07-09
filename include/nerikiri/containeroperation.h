#pragma once

#include <mutex>
#include <thread>

#include "nerikiri/value.h"
#include "nerikiri/object.h"
#include "nerikiri/operation.h"
#include "nerikiri/naming.h"

#include "nerikiri/containeroperationbase.h"

namespace nerikiri {


    template<typename T>
    class ContainerOperation : public nerikiri::ContainerOperationBase {
    private:
        std::function<Value(T&,Value)> function_;
        std::mutex mutex_;
    public:
        ContainerOperation(): ContainerOperationBase(true) {}
        ContainerOperation(const Value& info, const std::function<Value(T&, Value)>& func): ContainerOperationBase(info), function_(func){}

        virtual ~ContainerOperation() {}

        virtual Value getContainerOperationInfo() const override { return this->info(); }

        virtual Value call(const Value& value) override {
            std::lock_guard<std::mutex> lock(mutex_);
            if (this->isNullContainerOperation()) return Value::error("ContainerOperation is Null.");
            return this->function_(*((static_cast<Container<T>*>(container_))->ptr()), value);
        }

        virtual void setContainer(ContainerBase* container) override { 
            nerikiri::ContainerOperationBase::setContainer(container);
            this->info_["operationName"] = this->info_["typeName"];
            this->info_["ownerContainerInstanceName"] = container->info().at("instanceName");
            this->info_["ownerContainerTypeName"] = container->info().at("typeName");
            //this->info_["name"] = container->info().at("typeName").stringValue() + ":" + this->info_["typeName"].stringValue();
         }
    };






}