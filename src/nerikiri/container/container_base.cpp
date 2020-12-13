#include <string>
#include <vector>


#include <nerikiri/container_api.h>
#include "container_operation_base.h"
#include "container_factory_base.h"



#include "container_operation_base_impl.h"


using namespace nerikiri;

/**
 * ContainerBaseクラス
 * 
 * Containerの元になるクラス．メンバーOperationの管理などを担う
 */
class ContainerBase : public ContainerAPI {
protected:
    std::vector<std::shared_ptr<OperationAPI>> operations_;
    ContainerFactoryAPI* parentFactory_;
public:

    /**
     * コンストラクタ．実体を作る時はこちらのコンストラクタを使います．
     */
    ContainerBase(ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName) :  
        ContainerAPI(className, typeName, fullName), parentFactory_(parentFactory) {}

    /**
     * デストラクタ
     */
    virtual ~ContainerBase() {}


    /**
     * ContainerにOperationを登録します．登録時にOperationのinstanceNameおよびfullNameを更新します
     * 
     * @param operation
     */
    virtual Value addOperation(const std::shared_ptr<OperationAPI>& _operation) override { 
        logger::trace("ContainerBase::addOperation({})", _operation ? _operation->fullName() : "nullptr");
        if (!_operation) {
            return Value::error(logger::error("ContainerBase({})::addOperation(op) failed. Passed ContainerOperation is nullptr.", fullName()));
        }
        operations_.push_back((_operation)); 
        return _operation->info();;
    }

    virtual Value info() const override {
        auto inf = ContainerAPI::info();
        inf["operations"] = nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(operations(), [](auto op) {
            return op->info();
        });
        return inf;
    }

    /**
     * 登録しているOperationを削除します
     * 
     * @param opInfo
     * @returns
     */
    virtual Value deleteOperation(const std::string& fullName) override { 
        auto it = operations_.begin();
        for(;it != operations_.end();++it) {
            auto op = *it;
            
            if (op->info().at("fullName").stringValue() == fullName) {
                // match operation
                it = operations_.erase(it);
                return op->info();
            }
        }
        return Value::error("ContainerBase::deleteOperation() failed. Operation not found.");        
    }
    

    
    virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override { 
        return {operations_.begin(), operations_.end()};
        //return operations_;
    }

    virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const override {
        auto op = nerikiri::functional::find<std::shared_ptr<OperationAPI>>(operations(), [&fullName](auto op) { return op->fullName() == fullName; });
        if (op) return op.value();;
        return nullOperation();
    }

    /*
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


std::shared_ptr<ContainerAPI> nerikiri::containerBase(ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName) {
    return std::make_shared<ContainerBase>(parentFactory, className, typeName, fullName);
}
