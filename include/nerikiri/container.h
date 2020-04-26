#pragma once

#include "nerikiri/value.h"
#include "nerikiri/object.h"
#include "nerikiri/operationbase.h"
#include "nerikiri/naming.h"
#include "nerikiri/containeroperationfactorybase.h"
//#include "nerikiri/containeroperation.h"

namespace nerikiri {

    class ContainerOperationBase;
    class ContainerOperationFactoryBase;
    class ContainerFactoryBase;

    class OperationBase;

    inline bool containerNameValidator(const std::string& name) {
        if (name.find("/") != std::string::npos) {
        return false; // Invalid Name
        }
        if (name.find(":") != std::string::npos) {
        return false; // Invalid Name
        }
        return true;
    };

    class ContainerBase : public Object {
    protected:
        std::string type_;
        std::vector<std::shared_ptr<ContainerOperationBase>> operations_;
        ContainerFactoryBase* parentFactory_;

    public:
        std::string type() const { return type_; }

        static std::shared_ptr<ContainerBase> null;
    public:

        ContainerBase();

        ContainerBase(ContainerFactoryBase* parentFactory, const std::string& typeName, const Value& info);

        virtual ~ContainerBase() {}

        Value addOperation(std::shared_ptr<ContainerOperationBase> operation);

        std::vector<Value> getOperationInfos() const;

        std::shared_ptr<OperationBase> getOperation(const Value& info) const;

        std::shared_ptr<ContainerOperationFactoryBase> getContainerOperationFactory(const Value& info);

        Value createContainerOperation(const Value& info);

    };

    class ContainerOperationBase : public nerikiri::OperationBase {
    private:
        bool is_null_container_operation_;
        
    protected:
        ContainerBase* container_;

    public:
        ContainerOperationBase(bool is_null);

        ContainerOperationBase(const Value& info);
    
        virtual ~ContainerOperationBase();
    
    public:
        virtual Value getContainerOperationInfo() const = 0;

        virtual bool isNullContainerOperation() const;

        virtual void setContainer(ContainerBase* container);
    };


    class ContainerFactoryBase {
    private:
      std::vector<std::shared_ptr<ContainerOperationFactoryBase>> operationFactories_;
    public:
      virtual ~ContainerFactoryBase() {}
    public:
      virtual std::string typeName() = 0;
    public:
      virtual std::shared_ptr<ContainerBase> create(const Value& info) = 0;
      virtual ContainerFactoryBase& addOperationFactory(std::shared_ptr<ContainerOperationFactoryBase> cof) { 
          operationFactories_.push_back(cof);
          return *this;
      }

      friend class ContainerBase;
    };

    class ContainerOperationFactoryBase {
    public:
      virtual ~ContainerOperationFactoryBase() {}
    public:
      virtual std::string containerTypeName() = 0;
      virtual std::string typeName() = 0;
    public:
      virtual std::shared_ptr<ContainerOperationBase> create(const Value& info) = 0;
    };


    template<typename T>
    class Container : public ContainerBase {
    private:
        std::shared_ptr<T> _ptr;

    public:
        Container(ContainerFactoryBase* parentFactory, const Value& info): ContainerBase(parentFactory, demangle(typeid(T).name()), info), _ptr(std::make_shared<T>()) {
            if (!containerNameValidator(info.at("name").stringValue())) {
                //logger::error("Container ({}) can not be created. Invalid name format.", str(info_));
                is_null_ = true;
            }
        }
        virtual ~Container() {}
 
        std::shared_ptr<T>& ptr() { return _ptr; }

        std::shared_ptr<T>& operator->() { return ptr(); }
    };

    inline ContainerBase::ContainerBase() : Object(), parentFactory_(nullptr), type_("NullContainer") {}
            
    inline ContainerBase::ContainerBase(ContainerFactoryBase* parentFactory, const std::string& typeName, const Value& info) : parentFactory_(parentFactory), type_(typeName), Object(info) { }

    inline Value ContainerBase::createContainerOperation(const Value& info) {
        //logger::trace("ContainerBase::createContainerOperation({})", str(info));
        auto f = getContainerOperationFactory(info);
        if (!f) {
            return Value::error("ContainerBase::createContainerOperation failed. Can not find appropreate operation factory.");
        }
        //logger::info("Creating ContainerOperation({})", str(info));
        return addOperation(f->create(info));
    }

    inline Value ContainerBase::addOperation(std::shared_ptr<ContainerOperationBase> operation) { 
        auto name = nerikiri::numbering_policy<std::shared_ptr<ContainerOperationBase>>(operations_, operation->info().at("name").stringValue(), ".ope");
        operation->setInstanceName(name);
        if (!getOperation(operation->getContainerOperationInfo())->isNull()) {
            return Value::error("ContainerBase::addOperation failed. ContainerOperation with same name is registered. "+ str(operation->getContainerOperationInfo()));
        }
        operation->setContainer(this);
        operations_.push_back(operation); 
        return operation->info();;
    }

    inline std::shared_ptr<ContainerOperationFactoryBase> ContainerBase::getContainerOperationFactory(const Value& info) {
        if (this->isNull()) {
            //logger::error("ContainerBase::getContainerOperationFactory failed. Container is null.");
            return nullptr;
        }
        for(auto& opf : parentFactory_->operationFactories_) {
            if (opf->typeName() == opf->containerTypeName() + ":"+ info.at("name").stringValue()) {
            return opf;
            }
        }
        return nullptr;
    }


    inline std::vector<Value> ContainerBase::getOperationInfos() const {
        return nerikiri::map<Value, std::shared_ptr<ContainerOperationBase>>(operations_, [](auto op) { return op->getContainerOperationInfo();});
    }

    inline std::shared_ptr<OperationBase> ContainerBase::getOperation(const Value& info) const {
        for(auto op: operations_) {
            if (op->getContainerOperationInfo().at("instanceName") == info.at("instanceName")) {
                return op;
            }
        }
        return std::make_shared<OperationBase>(); // new OperationBase();
    }



inline ContainerOperationBase::ContainerOperationBase(bool is_null) : is_null_container_operation_(is_null) {}


inline ContainerOperationBase::ContainerOperationBase(const Value& info) : is_null_container_operation_(false), OperationBase(info) {}
    


inline ContainerOperationBase::~ContainerOperationBase() {}
    


inline bool ContainerOperationBase::isNullContainerOperation() const { return is_null_container_operation_; }


inline void ContainerOperationBase::setContainer(ContainerBase* container) { container_ = container; }


}