#pragma once

#include "nerikiri/value.h"
#include "nerikiri/object.h"
#include "nerikiri/operation.h"
#include "nerikiri/naming.h"

namespace nerikiri {

    class ContainerOperationBase : public nerikiri::OperationBase {
    private:
        bool is_null_container_operation_;
    public:
    ContainerOperationBase(bool is_null) : is_null_container_operation_(is_null) {}
    ContainerOperationBase(const Value& info) : is_null_container_operation_(false), OperationBase(info) {}
    
    virtual ~ContainerOperationBase() {}
    protected:
        ContainerBase* container_;
    public:
        virtual Value getContainerOperationInfo() const = 0;
        virtual bool isNullContainerOperation() const { return is_null_container_operation_; }
        virtual void setContainer(ContainerBase* container) { container_ = container; }

        static ContainerOperationBase *null;
    };

    template<typename T>
    class ContainerOperation : public nerikiri::ContainerOperationBase {
    private:
        std::function<Value(T&,Value)> function_;
    public:
        ContainerOperation(): ContainerOperationBase(true) {}
        ContainerOperation(const Value& info, const std::function<Value(T&, Value)>& func): ContainerOperationBase(info), function_(func){}

        virtual ~ContainerOperation() {}

        virtual Value getContainerOperationInfo() const override { return this->info(); }

        virtual Value call(const Value& value) override {
            if (this->isNullContainerOperation()) return Value::error("ContainerOperation is Null.");
            return this->function_(*((static_cast<Container<T>*>(container_))->ptr()), value);
        }

        virtual void setContainer(ContainerBase* container) override { 
            nerikiri::ContainerOperationBase::setContainer(container);
            this->info_["operationName"] = this->info_["name"];
            this->info_["name"] = container->info().at("name").stringValue() + ":" + this->info_["name"].stringValue();
         }

        virtual Value invoke() override {
            if (this->isNullContainerOperation()) {
                return Value::error("Opertaion is null");
            }
            return this->call(collectValues());// erikiri::call_operation(*this, this->collectValues());
        }
    };






}