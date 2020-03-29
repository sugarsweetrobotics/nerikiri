#pragma once

#include "nerikiri/value.h"
#include "nerikiri/operation.h"

namespace nerikiri {

    using ContainerInfo = nerikiri::Value;

    class ContainerOperationBase;
    using ContainerOperationBase_ptr = std::shared_ptr<ContainerOperationBase>;

    class ContainerBase {
    protected:
        ContainerInfo info_;
        std::string type_;
        std::vector<std::shared_ptr<ContainerOperationBase>> operations_;
        bool is_null_;

    public:
        const ContainerInfo& info() const { return info_; }
        std::string type() const { return type_; }

        bool isNull() const { return is_null_; }
    public:
        ContainerBase(const std::string& typeName, ContainerInfo&& info) : type_(typeName), info_(std::move(info)), is_null_(false) { }
        virtual ~ContainerBase() {}

        ContainerBase& addOperation(std::shared_ptr<ContainerOperationBase> operation);

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
    ContainerOperationBase(const Value& info) : is_null_container_operation_(false), OperationBaseBase(info) {}
    
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
        ContainerOperation(const OperationInfo& info, const std::function<Value(T&, Value)>& func): ContainerOperationBase(info), function_(func){}

        virtual ~ContainerOperation() {}

        virtual Value getContainerOperationInfo() const override { return this->info(); }

        virtual Value call(Value&& value) const override {
            if (this->isNullContainerOperation()) return Value::error("ContainerOperation is Null.");
            return this->function_(*((static_cast<Container<T>*>(container_))->ptr()), value);
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

    template<typename T>
    class ContainerFactory {
    public:
        ContainerFactory() {}
        ~ContainerFactory() {}

    public:
        std::shared_ptr<ContainerBase> create() { return std::shared_ptr<ContainerBase>(new Container<T>({{"name", typeid(T).name()}})); }
    };

    template<typename T>
    class ContainerOperationFactory {
    private:
        Value info_;
        std::function<Value(T&,Value&&)> function_;
    public:
        ContainerOperationFactory(const Value& info, std::function<Value(T&,Value&&)> func): info_(std::move(info)), function_(func) {}
        ~ContainerOperationFactory() {}

    public:
        std::shared_ptr<ContainerOperation<T>> create() { 
            return std::make_shared<ContainerOperation<T>>(info_, function_); 
        }
    };

}