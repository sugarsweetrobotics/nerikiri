#pragma once

#include "nerikiri/value.h"
#include "nerikiri/object.h"
#include "nerikiri/operation.h"
#include "nerikiri/naming.h"

namespace nerikiri {

    using ContainerInfo = nerikiri::Value;

    class ContainerOperationBase;
    using ContainerOperationBase_ptr = std::shared_ptr<ContainerOperationBase>;

    class ContainerOperationFactoryBase;
    class ContainerFactoryBase;

    class ContainerBase : public Object {
    protected:
        std::string type_;
        std::vector<std::shared_ptr<ContainerOperationBase>> operations_;
        ContainerFactoryBase* parentFactory_;
        bool is_null_;

    public:
        std::string type() const { return type_; }

        bool isNull() const { return is_null_; }

    public:
        ContainerBase(ContainerFactoryBase* parentFactory, const std::string& typeName, ContainerInfo&& info) : parentFactory_(parentFactory), type_(typeName), Object(info), is_null_(false) { }
        virtual ~ContainerBase() {}

        Value addOperation(std::shared_ptr<ContainerOperationBase> operation);

        std::vector<Value> getOperationInfos() const;

        ContainerOperationBase& getOperation(const Value& info) const;

        static ContainerBase null;

        std::shared_ptr<ContainerOperationFactoryBase> getContainerOperationFactory(const Value& info);

        Value createContainerOperation(const Value& info);

    };

    template<typename T>
    class Container : public ContainerBase {
    private:
        std::shared_ptr<T> _ptr;

    public:
        Container(ContainerFactoryBase* parentFactory, ContainerInfo&& info): ContainerBase(parentFactory, demangle(typeid(T).name()), std::move(info)), _ptr(std::make_shared<T>()) {
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


    class ContainerFactoryBase {
    private:
      std::vector<std::shared_ptr<ContainerOperationFactoryBase>> operationFactories_;
    public:
      virtual ~ContainerFactoryBase() {}
    public:
      virtual std::string typeName() = 0;
    public:
      virtual std::shared_ptr<ContainerBase> create() = 0;
      virtual ContainerFactoryBase& addOperationFactory(std::shared_ptr<ContainerOperationFactoryBase> cof) { 
          operationFactories_.push_back(cof);
          return *this;
      }

      friend class ContainerBase;
    };

    template<typename T>
    class ContainerFactory : public ContainerFactoryBase {
    public:
        ContainerFactory() {}
        virtual ~ContainerFactory() {}
    public:
        virtual std::string typeName() override { return demangle(typeid(T).name()); }
    public:
        virtual std::shared_ptr<ContainerBase> create() override { 
          return std::shared_ptr<ContainerBase>(new Container<T>(this, {{"name", demangle(typeid(T).name())}})); 
        }
    };

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

}