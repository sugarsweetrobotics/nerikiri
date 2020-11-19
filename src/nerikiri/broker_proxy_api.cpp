

#include <nerikiri/broker_proxy_api.h>

using namespace nerikiri;

class NullFactoryBroker : public FactoryBrokerAPI {
public:
    virtual ~NullFactoryBroker() {}
public:
    virtual Value createObject(const std::string& className, const Value& info={}) override {
        return Value::error(logger::error("NullFactoryBroker::createObject({}, {}) called. Object is null.", className, info));
    }

    virtual Value deleteObject(const std::string& className, const std::string& fullName) override {
        return Value::error(logger::error("NullFactoryBroker::createObject({}, {}) called. Object is null.", className, fullName));
    }
};

class NullStoreBroker : public StoreBrokerAPI {
public:
    virtual ~NullStoreBroker() {}

    virtual Value getObjectInfo(const std::string& className, const std::string& fullName) const override {
        return Value::error(logger::error("NullStoreBroker::getObjectInfo({}, {}) called. Object is null.", className, fullName));
    }

    virtual Value getClassObjectInfos(const std::string& className) const override {
        return Value::error(logger::error("NullStoreBroker::getClassObjectInfos({}) called. Object is null.", className));
    }

    virtual Value getChildrenClassObjectInfos(const std::string& parentName, const std::string& className) const override {
        return Value::error(logger::error("NullStoreBroker::getCildrenClassObjectInfos({}, {}) called. Object is null.", parentName, className));
    }
};


class NullOperationBroker : public OperationBrokerAPI {
public:
    NullOperationBroker() : OperationBrokerAPI()  {}
    virtual ~NullOperationBroker() {}

    virtual Value fullInfo(const std::string& fullName) const override {
        return Value::error(logger::error("NullOperationBroker::{}({}) called. Object is null.", __func__, fullName));
    }

    virtual Value call(const std::string& fullName, const Value& value) override {
        return Value::error(logger::error("NullOperationBroker::{}({}) called. Object is null.", __func__, fullName));
    }

    virtual Value invoke(const std::string& fullName) override {
        return Value::error(logger::error("NullOperationBroker::{}({}) called. Object is null.", __func__, fullName));
    }

    virtual Value execute(const std::string& fullName)  override {
        return Value::error(logger::error("NullOperationBroker::{}({}) called. Object is null.", __func__, fullName));
    }

    // collect informations of inlets ex., names.
    virtual Value inlets(const std::string& fullName) const  override {
        return Value::error(logger::error("NullOperationBroker::{}({}) called. Object is null.", __func__, fullName));
    }
};

class NullOperationOutletBroker : public OperationOutletBrokerAPI {
public:
    NullOperationOutletBroker(){}
    virtual ~NullOperationOutletBroker() {}

    virtual Value get(const std::string& fullName) const override {
        return Value::error(logger::error("NullOperationOutletBroker::{}({}) called. Object is null.", __func__, fullName));
    }

    virtual Value connections(const std::string& fullName) const override {
        return Value::error(logger::error("NullOperationOutletBroker::{}({}) called. Object is null.", __func__, fullName));
    }

    virtual Value addConnection(const std::string& fullName, const Value& c) override {
        return Value::error(logger::error("NullOperationOutletBroker::{}({}) called. Object is null.", __func__, fullName));
    }
    
    virtual Value removeConnection(const std::string& fullName, const std::string& name) override {
        return Value::error(logger::error("NullOperationOutletBroker::{}({}) called. Object is null.", __func__, fullName));
    }

    virtual Value info(const std::string& fullName) const override {
        return Value::error(logger::error("NullOperationOutletBroker::{}({}) called. Object is null.", __func__, fullName));
    }
};

class NullOperationInletBroker : public OperationInletBrokerAPI {
public:
    NullOperationInletBroker() {}
    virtual ~NullOperationInletBroker() {}

    virtual Value name(const std::string& fullName, const std::string& targetName) const override {
        return Value::error(logger::error("NullOperationInletBroker::{}({}, {}) called. Object is null.", __func__, fullName, targetName));
    }
    
    virtual Value defaultValue(const std::string& fullName, const std::string& targetName) const override {
        return Value::error(logger::error("NullOperationInletBroker::{}({}, {}) called. Object is null.", __func__, fullName, targetName));
    }

    virtual Value put(const std::string& fullName, const std::string& targetName, const Value& value) const override {
        return Value::error(logger::error("NullOperationInletBroker::{}({}, {}) called. Object is null.", __func__, fullName, targetName));
    }

    virtual Value get(const std::string& fullName, const std::string& targetName) const override {
        return Value::error(logger::error("NullOperationInletBroker::{}({}, {}) called. Object is null.", __func__, fullName, targetName));
    }

    virtual Value info(const std::string& fullName, const std::string& targetName) const override {
        return Value::error(logger::error("NullOperationInletBroker::{}({}, {}) called. Object is null.", __func__, fullName, targetName));
    }

    virtual Value isUpdated(const std::string& fullName, const std::string& targetName) const override {
        return Value::error(logger::error("NullOperationInletBroker::{}({}, {}) called. Object is null.", __func__, fullName, targetName));
    }

    virtual Value connections(const std::string& fullName, const std::string& targetName) const override {
        return Value::error(logger::error("NullOperationInletBroker::{}({}, {}) called. Object is null.", __func__, fullName, targetName));
    }

    virtual Value addConnection(const std::string& fullName, const std::string& targetName, const Value& c) override {
        return Value::error(logger::error("NullOperationInletBroker::{}({}, {}) called. Object is null.", __func__, fullName, targetName));
    }
    
    virtual Value removeConnection(const std::string& fullName, const std::string& targetName, const std::string& name) override {
        return Value::error(logger::error("NullOperationInletBroker::{}({}, {}) called. Object is null.", __func__, fullName, targetName));
    }
};

class NullBrokerProxy : public BrokerProxyAPI
{
private:

public:
    NullBrokerProxy(): BrokerProxyAPI("NullBrokerProxy", "null") {}
    virtual ~NullBrokerProxy() {}
public:

    virtual Value getProcessInfo() const override {
        return Value::error(logger::error("NullBrokerProxy::{} failed. Object is null.", __func__));
        
    }

    virtual Value getProcessFullInfo() const override {
        return Value::error(logger::error("NullBrokerProxy::{} failed. Object is null.", __func__));
        
    }

    virtual std::shared_ptr<FactoryBrokerAPI> factory() override {
        return std::make_shared<NullFactoryBroker>();
    }
    virtual std::shared_ptr<const FactoryBrokerAPI> factory() const override{
        return std::make_shared<NullFactoryBroker>();
    }

    virtual std::shared_ptr<StoreBrokerAPI> store() override {
        return std::make_shared<NullStoreBroker>();
    }
    virtual std::shared_ptr<const StoreBrokerAPI> store() const override {
        return std::make_shared<NullStoreBroker>();
    }


    virtual std::shared_ptr<OperationBrokerAPI>   operation() override {
        return std::make_shared<NullOperationBroker>();
    }
    virtual std::shared_ptr<const OperationBrokerAPI>   operation() const override {
        return std::make_shared<NullOperationBroker>();
    }

    virtual std::shared_ptr<OperationOutletBrokerAPI>   operationOutlet() override {
        return std::make_shared<NullOperationOutletBroker>();
    }
    virtual std::shared_ptr<const OperationOutletBrokerAPI>   operationOutlet() const override {
        return std::make_shared<NullOperationOutletBroker>();
    }

    virtual std::shared_ptr<OperationInletBrokerAPI>   operationInlet() override {
        return std::make_shared<NullOperationInletBroker>();
    }

    virtual std::shared_ptr<const OperationInletBrokerAPI>   operationInlet() const override {
        return std::make_shared<NullOperationInletBroker>();
    }
};




std::shared_ptr<BrokerProxyAPI> nerikiri::nullBrokerProxy() {
    return std::make_shared<NullBrokerProxy>();
}