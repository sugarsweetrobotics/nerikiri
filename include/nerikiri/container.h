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

    public:
        std::string type() const { return type_; }

        static std::shared_ptr<ContainerBase> null;
    public:

        ContainerBase() : Object(), parentFactory_(nullptr), type_("NullContainer") {}
        ContainerBase(ContainerFactoryBase* parentFactory, const std::string& typeName, const ContainerInfo& info) : parentFactory_(parentFactory), type_(typeName), Object(info) { }
        virtual ~ContainerBase() {}

        Value addOperation(std::shared_ptr<ContainerOperationBase> operation);

        std::vector<Value> getOperationInfos() const;

        std::shared_ptr<OperationBase> getOperation(const Value& info) const;

        std::shared_ptr<ContainerOperationFactoryBase> getContainerOperationFactory(const Value& info);

        Value createContainerOperation(const Value& info);

    };

    template<typename T>
    class Container : public ContainerBase {
    private:
        std::shared_ptr<T> _ptr;

    public:
        Container(ContainerFactoryBase* parentFactory, const ContainerInfo& info): ContainerBase(parentFactory, demangle(typeid(T).name()), info), _ptr(std::make_shared<T>()) {
            if (!nameValidator(info.at("name").stringValue())) {
                logger::error("Container ({}) can not be created. Invalid name format.", str(info_));
                is_null_ = true;
            }
        }
        virtual ~Container() {}
 
        std::shared_ptr<T>& ptr() { return _ptr; }

        std::shared_ptr<T>& operator->() { return ptr(); }
    };


}