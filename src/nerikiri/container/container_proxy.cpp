#include "container_proxy.h"

#include <juiz/container_api.h>
#include <juiz/client_proxy_api.h>

#include "../operation/operation_proxy.h"

using namespace nerikiri;

class ContainerProxy : public ContainerAPI {
private:
    const std::shared_ptr<ClientProxyAPI> broker_;
    const std::string fullName_;
    std::vector<OperationAPI> operations_;
public:
    ContainerProxy(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName) 
     : ContainerAPI("ContainerProxy", "Proxy", fullName), broker_(broker), fullName_(fullName) {}

    virtual ~ContainerProxy() {}
public:
    virtual Value info() const override {
        logger::trace("ContainerProxy::info() called");
        auto i = broker_->store()->getObjectInfo("container", fullName_);
        i["broker"] = broker_->fullInfo();
        logger::trace("ContainerProxy::info() exited");
        return i;
    }

    virtual Value fullInfo() const override {
        auto i = broker_->container()->fullInfo(fullName_);
        i["broker"] = broker_->fullInfo();
        return i;
    }


    virtual TimedPose3D getPose() const override {

    }

    virtual void setPose(const TimedPose3D& pose) override {

    }

    virtual void setPose(TimedPose3D&& pose) override {

    }

    virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override {
        return broker_->container()->operations(fullName_).const_list_map<std::shared_ptr<OperationAPI>>([this](const Value& v) {
            return operationProxy(broker_, Value::string(v["fullName"]));
        });
        //return operations_;
    }

    virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const override {
        //auto ops = std::find_if(operations_.begin(), operations_.end(), [](auto op) {
        //    return op->fullName() == op;
        //});
        //if (ops.size() > 0) return ops[0];
        auto operations = broker_->container()->operations(fullName_).const_list_map<std::shared_ptr<OperationAPI>>([this](const Value& v) {
            return operationProxy(broker_, Value::string(v["fullName"]));
        });
        auto ops = std::find_if(operations.begin(), operations.end(), [&fullName](auto op) {
            return op->fullName() == fullName;
        });
        if (ops != operations.end()) return *ops;
        return nullOperation();
    }

    virtual Value addOperation(const std::shared_ptr<OperationAPI>& operation) override {
        return Value::error(logger::error("ContainerProxy::addOperation() failed. Not implemented"));
    }

    virtual Value deleteOperation(const std::string& fullName) override {
        return Value::error(logger::error("ContainerProxy::deleteOperation() failed. Not implemented"));
    }
};




std::shared_ptr<ContainerAPI> nerikiri::containerProxy(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName) {
    return std::make_shared<ContainerProxy>(broker, fullName);
}
