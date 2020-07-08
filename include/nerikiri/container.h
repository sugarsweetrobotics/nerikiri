#pragma once

#include "nerikiri/value.h"
#include "nerikiri/object.h"
#include "nerikiri/naming.h"

#include "nerikiri/operationbase.h"
#include "nerikiri/containeroperationfactorybase.h"

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

        
    public:
        /**
         * コンストラクタ．Nullコンテナができます
         */
        ContainerBase() : Object(), parentFactory_(nullptr), type_("NullContainer") {}

        ContainerBase(ContainerFactoryBase* parentFactory, const std::string& typeName, const Value& info);

        virtual ~ContainerBase() {}

        Value addOperation(std::shared_ptr<ContainerOperationBase> operation);

        Value deleteOperation(const Value& opInfo);

        std::vector<Value> getOperationInfos() const;

        std::vector<std::shared_ptr<ContainerOperationBase>> getOperations() const {
            auto vs = operations_;
            return vs;
        }

        std::shared_ptr<OperationBase> getOperation(const Value& info) const;

        std::shared_ptr<ContainerOperationFactoryBase> getContainerOperationFactory(const Value& info);

        Value createContainerOperation(const Value& info);
        Value deleteContainerOperation(const Value& info);


        virtual Value setFullName(const std::string& nameSpace, const std::string& name) override;
    };

    inline static std::shared_ptr<ContainerBase> nullContainer() {
      return std::make_shared<ContainerBase>();
    }


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
            if (!containerNameValidator(info.at("typeName").stringValue())) {
                is_null_ = true;
            }
        }
        virtual ~Container() {}
 
        std::shared_ptr<T>& ptr() { return _ptr; }

        std::shared_ptr<T>& operator->() { return ptr(); }
    };

            
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

     inline Value ContainerBase::deleteContainerOperation(const Value& info) {
        auto f = getOperation(info);
        if (!f) {
            return Value::error("ContainerBase::deleteContainerOperation failed. Can not find appropreate operation factory.");
        }
        return deleteOperation(info);
    }


    inline Value ContainerBase::addOperation(std::shared_ptr<ContainerOperationBase> operation) { 

        if (operation->getInstanceName() == "") {
          auto name = nerikiri::numbering_policy<std::shared_ptr<ContainerOperationBase>>(operations_, operation->info().at("typeName").stringValue(), ".ope");
          operation->setFullName(getFullName(), name);
        } else {
          if (!getOperation(operation->info())->isNull()) {
            /// 重複があるようなのでエラーを返す
            for(auto& o: operations_) {
                if (operation->info().at("instanceName") == o->info().at("instanceName")) {
                    return Value::error("ContaienrBase::addOperation(" + operation->info().at("instanceName").stringValue() + ") Error. Process already has the same instanceName operation");
                }
            }
          } 
          operation->setFullName(getFullName(), operation->getInstanceName());
        }
        operation->setContainer(this);
        operations_.push_back(operation); 
        return operation->info();;
    }

    inline Value ContainerBase::deleteOperation(const Value& opInfo) { 
        auto it = operations_.begin();
        for(;it != operations_.end();++it) {
            auto op = *it;
            auto instanceName = opInfo.at("fullName").stringValue();
            
            if (op->info().at("fullName").stringValue() == instanceName) {
                // match operation
                it = operations_.erase(it);
                return opInfo;
            }
        }
        return Value::error("ContainerBase::deleteOperation() failed. Operation not found.");        
    }

    inline std::shared_ptr<ContainerOperationFactoryBase> ContainerBase::getContainerOperationFactory(const Value& info) {
        if (this->isNull()) {
            //logger::error("ContainerBase::getContainerOperationFactory failed. Container is null.");
            return nullptr;
        }
        for(auto& opf : parentFactory_->operationFactories_) {
            if (opf->typeName() == opf->containerTypeName() + ":"+ info.at("typeName").stringValue()) {
                return opf;
            }
        }
        return nullptr;
    }

    inline Value ContainerBase::setFullName(const std::string& nameSpace, const std::string& name) {
        Object::setFullName(nameSpace, name);
        for(auto& op: operations_) {
            op->setFullName(getFullName(), op->getInstanceName());
            op->setContainer(this);
        }
        return this->info();
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