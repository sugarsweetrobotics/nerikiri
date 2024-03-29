#include <string>
#include <iostream>
#include <vector>

#include <juiz/utils/functional.h>

#include <juiz/container_api.h>
#include "container_factory_base.h"


using namespace juiz;


namespace {
    auto daruma_mesh = Value{
            {"name", "modelData"},
            {"links", Value::valueList(
                Value{
                    {"name", "root"},
                    {"visual", {
                        {"geometry", {
                            {"box", {
                                {"size", Value::valueList(0.5, 0.5, 0.5)}
                            }}
                        }},
                        {"material", {
                            {"script", "red"}
                        }}
                    }},
                    {"pose", Value::valueList(0., 0., 0., 0., 0., 0.)}
                },
                Value{
                    {"name", "neck"},
                    {"visual", {
                        {"geometry", {
                            {"box", {
                                {"size", Value::valueList(0.3, 0.3, 0.3)}
                            }}
                        }},
                        {"material", {
                            {"script", "red"}
                        }}
                    }},
                    {"pose", Value::valueList(0., 0., 0.5, 0., 0., 0.)}
                }
            )}
        };

    auto default_mesh = Value{
            {"name", "modelData"},
            {"links", Value::valueList(
                Value{
                    {"name", "root"},
                    {"visual", {
                        {"geometry", {
                            {"box", {
                                {"size", Value::valueList(0.5, 0.5, 0.5)}
                            }}
                        }},
                        {"material", {
                            {"script", "red"}
                        }}
                    }},
                    {"pose", Value::valueList(0., 0., 0., 0., 0., 0.)}
                }
            )}
        };
}


/**
 * ContainerBaseクラス
 * 
 * Containerの元になるクラス．メンバーOperationの管理などを担う
 */
class ContainerBase : public ContainerAPI {
protected:
    std::vector<std::shared_ptr<OperationAPI>> operations_;

    const ContainerFactoryAPI* parentFactory_;

    Value mesh_;
    
    juiz::TimedPose3D pose_;

public:
    virtual TimedPose3D getPose() const override { return pose_; }

    virtual void setPose(const TimedPose3D& pose) override { pose_ = pose; }

    virtual void setPose(TimedPose3D&& pose) override { pose_ = std::move(pose); }

    virtual void setMeshData(const JUIZ_MESH_DATA& mesh) override {
        mesh_ = mesh;
    }
    
    virtual JUIZ_MESH_DATA getMeshData() const override {
        return mesh_;
    }

public:

    /**
     * コンストラクタ．実体を作る時はこちらのコンストラクタを使います．
     */
    ContainerBase(const ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName) :  
        ContainerAPI(className, typeName, fullName), parentFactory_(parentFactory), mesh_(default_mesh)
    {
    }

    /**
     * コンストラクタ．実体を作る時はこちらのコンストラクタを使います．
     */
    ContainerBase(const ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName, const Value& mesh) :  
        ContainerAPI(className, typeName, fullName), parentFactory_(parentFactory), mesh_(mesh)
    {
    }

    virtual void finalize() override {
        logger::trace_object to("ContainerBase(fullName={})::finalize() called.", fullName());
        auto ops = operations();
        std::for_each(ops.begin(), ops.end(), [this](auto op) {
            this->deleteOperation(op->fullName());
            op->finalize();
        });
        operations_.clear();
    }

    /**
     * デストラクタ
     */
    virtual ~ContainerBase() {
        logger::info("ContainerBase(fullName={})::~ContainerBase() called", fullName());


        logger::info("ContainerBase(fullName={})::~ContainerBase() exit", fullName());
    }

    /**
     * ContainerにOperationを登録します．登録時にOperationのinstanceNameおよびfullNameを更新します
     * 
     * @param operation
     */
    virtual Value addOperation(const std::shared_ptr<OperationAPI>& _operation) override { 
        logger::trace2_object to("ContainerBase::addOperation(operation(fullName={}, typeName={})) called", _operation->fullName(), _operation->typeName());
        if (_operation->isNull()) {
            return Value::error(logger::warn("ContainerBase::addOperation() failed. The argument operation is NullOperation. Ignored."));
        }
        logger::info("ContainerBase(fullName={})::addOperation(typeName={}, fullName={})", fullName(), _operation->typeName(), _operation->fullName());
        operations_.push_back((_operation)); 
        return _operation->info();;
    }

    virtual Value fullInfo() const override {
        auto inf = info();
        inf["operations"] = juiz::functional::map<Value, std::shared_ptr<OperationAPI>>(operations(), [](auto op) {
            return op->fullInfo();
        });
        inf["meshData"] = getMeshData();
        return inf;
    }

    virtual Value info() const override {
        auto inf = ContainerAPI::info();
        inf["basePose"] = juiz::toValue(this->getPose());
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
                logger::info("ContainerBase(fullName={})::deleteOperation(typeName={}, fullName={})", this->fullName(), op->typeName(), op->fullName());
                // match operation
                it = operations_.erase(it);
                return op->info();
            }
        }
        return Value::error("ContainerBase::deleteOperation() failed. Operation not found.");        
    }
    
    virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override { 
        return {operations_.begin(), operations_.end()};
    }

    virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const override {
        auto op = juiz::functional::find<std::shared_ptr<OperationAPI>>(operations(), [&fullName](auto op) { return op->fullName() == fullName; });
        if (op) return op.value();;

        auto opi = juiz::functional::find<std::shared_ptr<OperationAPI>>(operations(), [&fullName](auto op) { return op->instanceName() == fullName; });
        if (opi) return opi.value();;
        return nullOperation();
    }

};

std::shared_ptr<ContainerAPI> juiz::containerBase(const ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName) {
    return std::make_shared<ContainerBase>(parentFactory, className, typeName, fullName);
}

std::shared_ptr<ContainerAPI> juiz::containerBase(const ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName, const Value& mesh) {
    return std::make_shared<ContainerBase>(parentFactory, className, typeName, fullName, mesh);
}
