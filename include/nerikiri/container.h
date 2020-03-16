#pragma once

#include "nerikiri/value.h"
#include "nerikiri/operation.h"

namespace nerikiri {

    using ContainerInfo = nerikiri::Value;

    class ContainerOperationBase;

    class ContainerBase {
    protected:
        ContainerInfo info_;
        std::string type_;
        std::vector<ContainerOperationBase*> operations_;
        bool is_null_;

    public:
        const ContainerInfo& info() const { return info_; }
        std::string type() const { return type_; }

        bool isNull() const { return is_null_; }
    public:
        ContainerBase(const std::string& typeName, ContainerInfo&& info) : type_(typeName), info_(std::move(info)), is_null_(false) { }
        virtual ~ContainerBase() {}

        ContainerBase& addOperation(ContainerOperationBase* operation);

        std::vector<Value> getOperationInfos() const;

        ContainerOperationBase& getOperation(const Value& info) const;

        static ContainerBase null;
    };

    template<typename T>
    class Container : public ContainerBase {
    private:
        std::shared_ptr<T> _ptr;
    public:
        Container(ContainerInfo&& info): ContainerBase(typeid(T).name(), std::move(info)), _ptr(std::make_shared<T>()) {
            if (!nameValidator(info.at("name").stringValue())) {
                logger::error("Container ({}) can not be created. Invalid name format.", str(info_));
                is_null_ = true;
            }
        }
        virtual ~Container() {}
 
        std::shared_ptr<T>& ptr() { return _ptr; }

        std::shared_ptr<T>& operator->() { return ptr(); }
    };

    class ContainerOperationBase : public nerikiri::OperationBaseBase {
    private:
        bool is_null_container_operation_;
    public:
    ContainerOperationBase(bool is_null) : is_null_container_operation_(is_null) {}
    ContainerOperationBase(Value&& info) : is_null_container_operation_(false), OperationBaseBase(std::move(info)) {}
    
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
        std::function<Value(Container<T>&,Value)> function_;
    public:
        ContainerOperation(): ContainerOperationBase(true) {}
        ContainerOperation(OperationInfo&& info, std::function<Value(Container<T>&, Value)>&& func): ContainerOperationBase(std::move(info)), function_(std::move(func)){}

        virtual ~ContainerOperation() {}

        virtual Value getContainerOperationInfo() const override { return this->info(); }

        virtual Value call(Value&& value) const override {
            if (this->isNullContainerOperation()) return Value::error("ContainerOperation is Null.");
            return this->function_(*(static_cast<Container<T>*>(container_)), value);
        }

        virtual void setContainer(ContainerBase* container) override { 
            nerikiri::ContainerOperationBase::setContainer(container);
            this->info_["operationName"] = this->info_["name"];
            this->info_["name"] = container->info().at("name").stringValue() + ":" + this->info_["name"].stringValue();
         }

        virtual Value invoke() const override {
            if (this->isNullContainerOperation()) {
                return Value::error("Opertaion is null");
            }
            return nerikiri::call_operation(*this, this->collectValues());
        }
    };


}