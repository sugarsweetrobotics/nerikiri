#pragma once

#include <string>
#include <vector>
#include "nerikiri/value.h"
#include "nerikiri/object.h"

#include "nerikiri/containeroperationbase.h"
#include "nerikiri/containerfactorybase.h"
#include "nerikiri/containeroperationbase.h"

namespace nerikiri {

    /**
     * ContainerBaseクラス
     * 
     * Containerの元になるクラス．メンバーOperationの管理などを担う
     */
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

        /**
         * コンストラクタ．実体を作る時はこちらのコンストラクタを使います．
         */
        ContainerBase(ContainerFactoryBase* parentFactory, const std::string& typeName, const Value& info) :  parentFactory_(parentFactory), type_(typeName), Object(info) {}

        /**
         * デストラクタ
         */
        virtual ~ContainerBase() {}

        /**
         * ContainerにOperationを登録します．登録時にOperationのinstanceNameおよびfullNameを更新します
         * 
         * @param operation
         */
        Value addOperation(const std::shared_ptr<ContainerOperationBase>& operation) { 
            /// もしインスタンス名が設定されていなかったら
            if (operation->getInstanceName() == "") {
                auto name = nerikiri::numbering_policy<std::shared_ptr<ContainerOperationBase>>(operations_, operation->info().at("typeName").stringValue(), ".ope");
                operation->setFullName(getFullName(), name);
            } else {
                if (!getOperation(operation->info().at("instanceName").stringValue())->isNull()) {
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

        /**
         * 登録しているOperationを削除します
         * 
         * @param opInfo
         * @returns
         */
        Value deleteOperation(const std::string& instanceName) { 
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

        /**
         * 登録されている全てのContainerOperationのinfoを取得します
         * 
         * @returns
         */
        std::vector<Value> getOperationInfos() const {
            return nerikiri::map<Value, std::shared_ptr<ContainerOperationBase>>(operations_, [](auto op) { 
                return op->getContainerOperationInfo();
            });
        }

        /**
         * 登録されている全てのContainerOperationへのポインタをvectorで返します
         */
        std::vector<std::shared_ptr<ContainerOperationBase>> getOperations() const {
            return operations_;
        }

        /**
         * 登録されているContainerOperationの中から引数のinfoに該当するContainerOperationを取得します
         * 
         * @param info
         * @returns
         */
        std::shared_ptr<OperationBase> getOperation(const std::string& instanceName) const {
            for(auto op: operations_) {
                if (op->getContainerOperationInfo().at("instanceName").stringValue() == instanceName) {
                    return op;
                }
            }
            return std::make_shared<OperationBase>(); // new OperationBase();
        }

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

        Value createContainerOperation(const Value& info) {
            //logger::trace("ContainerBase::createContainerOperation({})", str(info));
            auto f = getContainerOperationFactory(info);
            if (!f) {
                return Value::error("ContainerBase::createContainerOperation failed. Can not find appropreate operation factory.");
            }
            //logger::info("Creating ContainerOperation({})", str(info));
            return addOperation(f->create(info));
        }

        Value deleteContainerOperation(const std::string& instanceName) {
            auto f = getOperation(instanceName);
            if (!f) {
                return Value::error("ContainerBase::deleteContainerOperation failed. Can not find appropreate operation factory.");
            }
            return deleteOperation(instanceName);
        }


        virtual Value setFullName(const std::string& nameSpace, const std::string& name) override  {
            Object::setFullName(nameSpace, name);
            for(auto& op: operations_) {
                op->setFullName(getFullName(), op->getInstanceName());
                op->setContainer(this);
            }
            return this->info();
        }
    };


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

}