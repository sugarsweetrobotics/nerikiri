#include <nerikiri/crud_broker_proxy_base.h>


using namespace nerikiri;


class CRUDFactoryBroker : public FactoryBrokerAPI {
private:
    CRUDBrokerProxyAPI* broker_;
public:
    virtual ~CRUDFactoryBroker() {}
    CRUDFactoryBroker(CRUDBrokerProxyAPI* broker) : FactoryBrokerAPI(), broker_(broker) {}

public:
    virtual Value createObject(const std::string& className, const Value& info={}) {
        return broker_->createResource(className + "s/", info);
    }

    virtual Value deleteObject(const std::string& className, const std::string& fullName) {
        return broker_->deleteResource(className + "s/" + fullName);
    }
};

class CRUDStoreBroker : public StoreBrokerAPI {
private:
    CRUDBrokerProxyAPI* broker_;
public:
    CRUDStoreBroker (CRUDBrokerProxyAPI* broker) : StoreBrokerAPI(), broker_(broker) {}
    virtual ~CRUDStoreBroker() {}

    virtual Value getObjectInfo(const std::string& className, const std::string& fullName) const {
        return broker_->readResource(className + "s/" + fullName);
    }

    virtual Value getClassObjectInfos(const std::string& className) const {
        return broker_->readResource(className + "s/");
    }

    virtual Value getChildrenClassObjectInfos(const std::string& parentName, const std::string& className) const {
        return {};
    }
};


class CRUDOperationBroker : public OperationBrokerAPI {
private:
    CRUDBrokerProxyAPI* broker_;
public:
    CRUDOperationBroker (CRUDBrokerProxyAPI* broker) : OperationBrokerAPI(), broker_(broker) {}
    virtual ~CRUDOperationBroker() {}

    
    virtual Value fullInfo(const std::string& fullName) const override {
        return broker_->readResource("operations/" + fullName + "/fullInfo");
    }

    virtual Value call(const std::string& fullName, const Value& value) override {
        return broker_->updateResource("operations/" + fullName + "/call", value);
    }

    virtual Value invoke(const std::string& fullName) override {
        return broker_->updateResource("operations/" + fullName + "/invoke", {});
    }

    virtual Value execute(const std::string& fullName) override {
        return broker_->updateResource("operations/" + fullName + "/execute", {});
    }
    // collect informations of inlets ex., names.
    virtual Value inlets(const std::string& fullName) const override {
        return broker_->readResource("operations/" + fullName + "/inlets");
    }
};

class CRUDOperationOutletBroker : public OperationOutletBrokerAPI {
    CRUDBrokerProxyAPI* broker_;
public:
    CRUDOperationOutletBroker(CRUDBrokerProxyAPI* broker) : broker_(broker) {}
    virtual ~CRUDOperationOutletBroker() {}

    virtual Value get(const std::string& fullName) const override {
        return broker_->readResource("operations/" + fullName + "/outlet");
    }

    virtual Value connections(const std::string& fullName) const override {
        return broker_->readResource("operations/" + fullName + "/outlet/connections");
    }

    virtual Value addConnection(const std::string& fullName, const Value& c) override {
        return broker_->createResource("operations/" + fullName + "/outlet/connections", c);
    }
    
    virtual Value removeConnection(const std::string& fullName, const std::string& name) override {
        return broker_->deleteResource("operations/" + fullName + "/outlet/connections/" + name);
    }

    virtual Value info(const std::string& fullName) const override {
        return broker_->readResource("operations/" + fullName + "/outlet/info");
    }
};

class CRUDOperationInletBroker : public OperationInletBrokerAPI {
    CRUDBrokerProxyAPI* broker_;
public:
    CRUDOperationInletBroker(CRUDBrokerProxyAPI* broker) : broker_(broker) {}
    virtual ~CRUDOperationInletBroker() {}

    virtual Value name(const std::string& fullName, const std::string& targetName) const override {
        return broker_->readResource("operations/" + fullName + "/inlets/" + targetName + "/name");
    }
    
    virtual Value defaultValue(const std::string& fullName, const std::string& targetName) const override {
        return broker_->readResource("operations/" + fullName + "/inlets/" + targetName + "/defaultValue");
    }

    virtual Value put(const std::string& fullName, const std::string& targetName, const Value& value) const override {
        return broker_->updateResource("operations/" + fullName + "/inlets/" + targetName, value);
    }

    virtual Value get(const std::string& fullName, const std::string& targetName) const override {
        return broker_->readResource("operations/" + fullName + "/inlets/" + targetName);
    }

    virtual Value info(const std::string& fullName, const std::string& targetName) const override {
        return broker_->readResource("operations/" + fullName + "/inlets/" + targetName + "/info");
    }

    virtual Value isUpdated(const std::string& fullName, const std::string& targetName) const override {
        return broker_->readResource("operations/" + fullName + "/inlets/" + targetName + "/isUpdated");
    }

    virtual Value connections(const std::string& fullName, const std::string& targetName) const override {
        return broker_->readResource("operations/" + fullName + "/inlets/" + targetName + "/connections");
    }

    virtual Value addConnection(const std::string& fullName, const std::string& targetName, const Value& c) override {
        return broker_->createResource("operations/" + fullName + "/inlets/" + targetName + "/connections", c);
    }
    
    virtual Value removeConnection(const std::string& fullName, const std::string& targetName, const std::string& name) override {
        return broker_->deleteResource("operations/" + fullName + "/inlets/" + targetName + "/connections/" + name);
    }
};

class CRUDConnectionBroker : public ConnectionBrokerAPI {
    CRUDBrokerProxyAPI* broker_;
public:
    CRUDConnectionBroker(CRUDBrokerProxyAPI* broker) : broker_(broker) {}
    virtual ~CRUDConnectionBroker() {}
public:
    virtual Value createConnection(const Value& connectionInfo) override {
        return broker_->createResource("connections/", connectionInfo);
    }
    virtual Value deleteConnection(const std::string& fullName) override {
        return broker_->readResource("connections/" + fullName);
    }
};


class CRUDContainerBroker : public ContainerBrokerAPI {
    CRUDBrokerProxyAPI* broker_;
public:
    CRUDContainerBroker(CRUDBrokerProxyAPI* broker) : broker_(broker) {}
    virtual ~CRUDContainerBroker() {}

    virtual Value operations(const std::string& containerFullName) const override {
        return broker_->readResource("containers/" + containerFullName + "/operations");
    }
};


class CRUDECBroker : public ECBrokerAPI {
private:
    CRUDBrokerProxyAPI* broker_;
public:
    CRUDECBroker(CRUDBrokerProxyAPI* broker) : broker_(broker) {}
    virtual ~CRUDECBroker() {}


    virtual Value activateStart(const std::string& fullName) const override {
        return Value::error(logger::error("NullECBroker::{}({}) called. Object is null.", __func__, fullName));
    }

    virtual Value activateStop(const std::string& fullName) override {
        return Value::error(logger::error("NullECBroker::{}({}) called. Object is null.", __func__, fullName));
    }

};



class CRUDFSMBroker : public FSMBrokerAPI {
private:
    CRUDBrokerProxyAPI* broker_;
public:
    CRUDFSMBroker(CRUDBrokerProxyAPI* broker) : broker_(broker) {}
    virtual ~CRUDFSMBroker() {}

    virtual Value currentFSMState(const std::string& fsmFullName) override {
        return Value::error(logger::error("NullFSMBroker::{}({}) called. Object is null.", __func__, fsmFullName));
    }

    virtual Value setFSMState(const std::string& fsmFullName, const std::string& stateFullName) override {
        return Value::error(logger::error("NullFSMBroker::{}({}) called. Object is null.", __func__, fsmFullName));
    }

    virtual Value fsmStates(const std::string& fsmFullName) override {
        return Value::error(logger::error("NullFSMBroker::{}({}) called. Object is null.", __func__, fsmFullName));
    }

    virtual Value fsmState(const std::string& fsmFullName, const std::string& stateName) {
        return Value::error(logger::error("NullFSMBroker::{}({}) called. Object is null.", __func__, fsmFullName));
    }
};

class CRUDFSMStateBroker : public FSMStateBrokerAPI {
private:
    CRUDBrokerProxyAPI* broker_;
public:
    CRUDFSMStateBroker(CRUDBrokerProxyAPI* broker) : broker_(broker) {}
virtual ~CRUDFSMStateBroker() {}

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
};



class CRUDFSMStateInletBroker : public OperationInletBrokerAPI {
private:

    CRUDBrokerProxyAPI* broker_;
public:
    CRUDFSMStateInletBroker(CRUDBrokerProxyAPI* broker) : broker_(broker) {}
public:
    virtual ~CRUDFSMStateInletBroker() {}

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

CRUDBrokerProxyBase::CRUDBrokerProxyBase(const std::string& typeName, const std::string& fullName) : 
  CRUDBrokerProxyAPI(), BrokerProxyAPI(typeName, fullName, 
    std::make_shared<CRUDStoreBroker>(this),
    std::make_shared<CRUDFactoryBroker>(this),
    std::make_shared<CRUDOperationBroker>(this),
    std::make_shared<CRUDOperationOutletBroker>(this),
    std::make_shared<CRUDOperationInletBroker>(this),
    std::make_shared<CRUDConnectionBroker>(this),
    std::make_shared<CRUDContainerBroker>(this),
    std::make_shared<CRUDECBroker>(this),
    std::make_shared<CRUDFSMBroker>(this),
    std::make_shared<CRUDFSMStateBroker>(this),
    std::make_shared<CRUDFSMStateInletBroker>(this)
    )
{}

CRUDBrokerProxyBase::~CRUDBrokerProxyBase() {}

Value CRUDBrokerProxyBase::getProcessInfo() const {
    return readResource("info");            
}

Value CRUDBrokerProxyBase::getProcessFullInfo() const {
    return readResource("fullInfo");            
}