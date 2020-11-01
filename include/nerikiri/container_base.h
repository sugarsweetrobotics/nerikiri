#pragma once

#include <string>
#include <vector>


#include <nerikiri/container_api.h>
#include <nerikiri/container_operation_base.h>
#include <nerikiri/container_factory_base.h>
#include <nerikiri/container_operation_base.h>

namespace nerikiri {

    /**
     * ContainerBaseクラス
     * 
     * Containerの元になるクラス．メンバーOperationの管理などを担う
     */
    class ContainerBase : public ContainerAPI {
    protected:
        std::string containerTypeName_;
        std::vector<std::shared_ptr<ContainerOperationBase>> operations_;
        ContainerFactoryBase* parentFactory_;

    public:
        std::string cotainerTypeName() const { return containerTypeName_; }
    public:

        /**
         * コンストラクタ．実体を作る時はこちらのコンストラクタを使います．
         */
        ContainerBase(ContainerFactoryBase* parentFactory, const std::string& typeName, const std::string& containerTypeName, const std::string& fullName) :  
          ContainerAPI(typeName, fullName), containerTypeName_(containerTypeName), parentFactory_(parentFactory) {}

        /**
         * デストラクタ
         */
        virtual ~ContainerBase() {}

        /**
         * ContainerにOperationを登録します．登録時にOperationのinstanceNameおよびfullNameを更新します
         * 
         * @param operation
         *
        virtual Value addOperation(const std::shared_ptr<OperationAPI>& _operation) override { 
            
            
            operations_.push_back(_operation); 
            return _operation->info();;
        }

         **
         * 登録しているOperationを削除します
         * 
         * @param opInfo
         * @returns
         *
        virtual Value deleteOperation(const std::string& instanceName) override { 
            auto it = operations_.begin();
            for(;it != operations_.end();++it) {
                auto op = *it;
                
                if (op->info().at("fullName").stringValue() == instanceName) {
                    // match operation
                    it = operations_.erase(it);
                    return op->info();
                }
            }
            return Value::error("ContainerBase::deleteOperation() failed. Operation not found.");        
        }
        */

        /*
        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override { 
            return {operations_.begin(), operations_.end()};
            //return operations_;
        }

        virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const override {
            auto op = nerikiri::functional::find<std::shared_ptr<OperationAPI>>(operations(), [&fullName](auto op) { return op->fullName() == fullName; });
            if (op) return op.value();;
            return std::make_shared<NullOperation>();
        }

        *
        std::shared_ptr<ContainerOperationFactoryBase> getContainerOperationFactory(const Value& info) {
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
        */

        /*
        Value createContainerOperation(const Value& info) {
            //logger::trace("ContainerBase::createContainerOperation({})", str(info));
            auto f = getContainerOperationFactory(info);
            if (!f) {
                return Value::error("ContainerBase::createContainerOperation failed. Can not find appropreate operation factory.");
            }
            //logger::info("Creating ContainerOperation({})", str(info));
            return addOperation(f->create(info));
        }
        */
        /*
        Value deleteContainerOperation(const std::string& fullName) {
            auto f = operation(fullName);
            if (!f) {
                return Value::error("ContainerBase::deleteContainerOperation failed. Can not find appropreate operation factory.");
            }
            return deleteOperation(fullName);
        }
        */

        /*
        virtual Value setFullName(const std::string& nameSpace, const std::string& name) override  {
            Object::setFullName(nameSpace, name);
            for(auto& op: operations_) {
                op->setFullName(getFullName(), op->getInstanceName());
                op->setContainer(this);
            }
            return this->info();
        }
        */
    };

    /*
    inline static std::shared_ptr<ContainerBase> nullContainer() {
      return std::make_shared<ContainerBase>();
    }

    inline bool containerNameValidator(const std::string& name) {
        if (name.find("/") != std::string::npos) {
        return false; // Invalid Name
        }
        if (name.find(":") != std::string::npos) {
        return false; // Invalid Name
        }
        return true;
    };
    */
}