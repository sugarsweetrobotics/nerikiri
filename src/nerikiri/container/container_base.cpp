#include <string>
#include <vector>

#include <juiz/utils/functional.h>

#include <juiz/container_api.h>
#include "container_factory_base.h"


using namespace nerikiri;

/**
 * ContainerBaseクラス
 * 
 * Containerの元になるクラス．メンバーOperationの管理などを担う
 */
class ContainerBase : public ContainerAPI {
protected:
    std::vector<std::shared_ptr<OperationAPI>> operations_;
    const ContainerFactoryAPI* parentFactory_;

    
    nerikiri::TimedPose3D pose_;

public:
    virtual TimedPose3D getPose() const override { return pose_; }
    virtual void setPose(const TimedPose3D& pose) override { pose_ = pose; }
    virtual void setPose(TimedPose3D&& pose) override { pose_ = std::move(pose); }
public:

    /**
     * コンストラクタ．実体を作る時はこちらのコンストラクタを使います．
     */
    ContainerBase(const ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName) :  
        ContainerAPI(className, typeName, fullName), parentFactory_(parentFactory)
    {
    }

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
        logger::trace("ContainerBase::addOperation({}) called", _operation ? _operation->fullName() : "nullptr");
        if (!_operation) {
            return Value::error(logger::error("ContainerBase({})::addOperation(op) failed. Passed ContainerOperation is nullptr.", fullName()));
        }
        logger::info("Container is adding a ContainerOperation(fullName={}, typeName=)", _operation->fullName(), _operation->typeName());
        operations_.push_back((_operation)); 
        return _operation->info();;
    }

    virtual Value fullInfo() const override {
        auto inf = info();
        inf["operations"] = nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(operations(), [](auto op) {
            return op->fullInfo();
        });
        return inf;
    }

    virtual Value info() const override {
        auto inf = ContainerAPI::info();
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

        auto opi = nerikiri::functional::find<std::shared_ptr<OperationAPI>>(operations(), [&fullName](auto op) { return op->instanceName() == fullName; });
        if (opi) return opi.value();;
        return nullOperation();
    }

};



std::shared_ptr<ContainerAPI> nerikiri::containerBase(const ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName) {
    return std::make_shared<ContainerBase>(parentFactory, className, typeName, fullName);
}
