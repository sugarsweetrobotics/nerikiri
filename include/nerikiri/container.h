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
    public:
        const ContainerInfo& info() const { return info_; }
        std::string type() const { return type_; }
    public:
        ContainerBase(const std::string& typeName, ContainerInfo&& info) : type_(typeName), info_(std::move(info)) {}
        virtual ~ContainerBase() {}

        ContainerBase& addOperation(ContainerOperationBase* operation);

        Value getOperationInfos() const;

        ContainerOperationBase& getOperation(const Value& info) const;

        static ContainerBase null;
    };

    template<typename T>
    class Container : public ContainerBase {
    private:
        std::shared_ptr<T> _ptr;
    public:
        Container(ContainerInfo&& info): ContainerBase(typeid(T).name(), std::move(info)), _ptr(std::make_shared<T>()) {
        }
        virtual ~Container() {}
 
        std::shared_ptr<T>& ptr() { return _ptr; }

        std::shared_ptr<T>& operator->() { return ptr(); }
    };

    class ContainerOperationBase : public nerikiri::Callable, public nerikiri::Invokable{
    private:
        bool is_null_container_operation_;
    public:
        ContainerOperationBase(bool is_null) : is_null_container_operation_(is_null) {}

        virtual ~ContainerOperationBase() {}
    protected:
        ContainerBase* container_;
    public:
        virtual Value getContainerOperationInfo() const = 0;
        virtual bool isNullContainerOperation() const { return is_null_container_operation_; }
        void setContainer(ContainerBase* container) { container_ = container; }

        static ContainerOperationBase *null;
    };

    template<typename T>
    class ContainerOperation : public nerikiri::ContainerOperationBase, public nerikiri::OperationBase<std::function<Value(Container<T>&,Value)>> {
    private:

    public:
        ContainerOperation(): ContainerOperationBase(false), OperationBase<std::function<Value(Container<T>&,Value)>>() {
        }
        ContainerOperation(OperationInfo&& info, std::function<Value(Container<T>&, Value)>&& func): ContainerOperationBase(true),
            OperationBase<std::function<Value(Container<T>&,Value)>>(std::move(info),std::move(func)) {
        }

        virtual ~ContainerOperation() {}

        virtual Value getContainerOperationInfo() const override { return this->info(); }

        virtual Value call(Value&& value) const override {
            if (this->isNull()) return Value::error("ContainerOperation is Null.");
            return this->function_(*(static_cast<Container<T>*>(container_)), value);
        }

        virtual Value invoke() const override {
            if (this->isNull()) {
                return Value::error("Opertaion is null");
            }
            return nerikiri::call_operation(*this, this->collectValues());
        }
    };


}