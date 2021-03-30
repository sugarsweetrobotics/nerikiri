

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

    virtual Value connectTo(const std::string& fullName, const Value& conInfo) override {
        return Value::error(logger::error("NullOperationOutletBroker::{}({}) called. Object is null.", __func__, fullName));
    }

    virtual Value disconnectFrom(const std::string& fullName, const Value& inletInfo) override {
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

    virtual Value connectTo(const std::string& fullName, const std::string& targetName, const Value& conInfo) override {
        return Value::error(logger::error("NullOperationInletBroker::{}({}) called. Object is null.", __func__, fullName));
    }

    virtual Value disconnectFrom(const std::string& fullName, const std::string& targetName, const Value& outletInfo) override {
        return Value::error(logger::error("NullOperationInletBroker::{}({}) called. Object is null.", __func__, fullName));
    }
};


class NullConnectionBroker : public ConnectionBrokerAPI {
public:
    virtual ~NullConnectionBroker() {}

    virtual Value createConnection(const Value& connectionInfo) override {
        return Value::error(logger::error("NullConnectionBroker::{}({}) called. Object is null.", __func__));
    }

    virtual Value deleteConnection(const std::string& fullName) override {
        return Value::error(logger::error("NullConnectionBroker::{}({}) called. Object is null.", __func__, fullName));
    }

};


class NullContainerBroker : public ContainerBrokerAPI {
public:
    virtual ~NullContainerBroker() {}

    virtual Value operations(const std::string& containerFullName) const override {
        return Value::error(logger::error("NullContainerBroker::{}({}) called. Object is null.", __func__, containerFullName));
    }

};

class NullECBroker : public ECBrokerAPI {
public:
    virtual ~NullECBroker() {}


    virtual Value activateStart(const std::string& fullName) const override {
        return Value::error(logger::error("NullECBroker::{}({}) called. Object is null.", __func__, fullName));
    }

    virtual Value activateStop(const std::string& fullName) override {
        return Value::error(logger::error("NullECBroker::{}({}) called. Object is null.", __func__, fullName));
    }
};


class NullFSMBroker : public FSMBrokerAPI {
public:
    virtual ~NullFSMBroker() {}

    virtual Value currentFSMState(const std::string& fsmFullName) override {
        return Value::error(logger::error("NullFSMBroker::{}({}) called. Object is null.", __func__, fsmFullName));
    }

    virtual Value setFSMState(const std::string& fsmFullName, const std::string& stateFullName) override {
        return Value::error(logger::error("NullFSMBroker::{}({}) called. Object is null.", __func__, fsmFullName));
    }

    virtual Value fsmStates(const std::string& fsmFullName) override {
        return Value::error(logger::error("NullFSMBroker::{}({}) called. Object is null.", __func__, fsmFullName));
    }

    virtual Value fsmState(const std::string& fsmFullName, const std::string& stateName) override {
        return Value::error(logger::error("NullFSMBroker::{}({}) called. Object is null.", __func__, fsmFullName));
    }
};

class NullFSMStateBroker : public FSMStateBrokerAPI {
public:
    virtual ~NullFSMStateBroker() {}

public:
    virtual Value isActive(const std::string& fsmName, const std::string& stateName) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }

    virtual Value activate(const std::string& fsmName, const std::string& stateName) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }

    virtual Value deactivate(const std::string& fsmName, const std::string& stateName) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }

    virtual Value isTransitable(const std::string& fsmName, const std::string& stateName,const std::string& targetStateName) const override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }

    virtual Value bindOperation(const std::string& fsmName, const std::string& stateName, const Value& info) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }

    virtual Value bindOperation(const std::string& fsmName, const std::string& stateName, const Value& info, const Value& arg) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }

    virtual Value bindECState(const std::string& fsmName, const std::string& stateName, const Value& info) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }

    virtual Value unbindOperation(const std::string& fsmName, const std::string& stateName, const Value& info) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }

    virtual Value unbindECState(const std::string& fsmName, const std::string& stateName, const Value& info) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }

    virtual Value boundOperations(const std::string& fsmName, const std::string& stateName) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }

    virtual Value boundECStates(const std::string& fsmName, const std::string& stateName) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }

    virtual Value inlet(const std::string& fsmName, const std::string& stateName) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }
    
    /*
    virtual Value bindOperation(const std::string& fsmName, const std::string& stateName, const Value& info) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));
    }

    virtual Value addConnection(const std::string& fsmName, const std::string& stateName, const Value& c) override {
        return Value::error(logger::error("NullFSMStateBroker::{}({}, {}) called. Object is null.", __func__, fsmName, stateName));       
    }
    */
};

class NullBrokerProxy : public BrokerProxyAPI
{
private:

public:
    NullBrokerProxy(): BrokerProxyAPI("NullBrokerProxy", "null", 
        std::make_shared<NullStoreBroker>(),
        std::make_shared<NullFactoryBroker>(),
        std::make_shared<NullOperationBroker>(),
        std::make_shared<NullOperationOutletBroker>(),
        std::make_shared<NullOperationInletBroker>(),
        std::make_shared<NullConnectionBroker>(),
        std::make_shared<NullContainerBroker>(),
        std::make_shared<NullECBroker>(),
        std::make_shared<NullFSMBroker>(),
        std::make_shared<NullFSMStateBroker>(),
        std::make_shared<NullOperationInletBroker>()
    ) {}
    virtual ~NullBrokerProxy() {}
public:

    virtual Value getProcessInfo() const override {
        return Value::error(logger::error("NullBrokerProxy::{} failed. Object is null.", __func__));
        
    }

    virtual Value getProcessFullInfo() const override {
        return Value::error(logger::error("NullBrokerProxy::{} failed. Object is null.", __func__));
        
    }
};




std::shared_ptr<BrokerProxyAPI> nerikiri::nullBrokerProxy() {
    return std::make_shared<NullBrokerProxy>();
}