#include <nerikiri/fsm_proxy.h>
#include "nerikiri/proxy_builder.h"


using namespace nerikiri;

class FSMStateProxy;
class FSMStateInletProxy;

class FSMStateInletProxy : public OperationInletAPI {
private:
    const std::shared_ptr<FSMStateProxy> state_;
public:
    FSMStateInletProxy(const std::shared_ptr<FSMStateProxy>& state) : OperationInletAPI(), state_(state) {}
    virtual ~FSMStateInletProxy() {}


    virtual bool isNull() const { return false; }
    
    virtual std::string name() const;
    
    
    virtual std::string ownerFullName() const;


    virtual Value defaultValue() const { return {}; }

    virtual Value get() const { return {}; }

    virtual Value put(const Value& value) { 
        return {}; 
    }

    virtual Value executeOwner();

    virtual Value info() const;

    virtual bool isUpdated() const { return false; }

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const;

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c);

    virtual Value removeConnection(const std::string& _fullName);
};

class FSMStateProxy : public FSMStateAPI {
public:
    const std::shared_ptr<BrokerProxyAPI> broker_;
    const std::string fsmFullName_;
    const std::string stateName_;
    std::shared_ptr<OperationInletAPI> inlet_;
public:
    FSMStateProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fsmFullName, const std::string& stateName) : FSMStateAPI("FSMStateProxy", fsmFullName, (stateName)), broker_(broker),
        fsmFullName_(fsmFullName), stateName_(stateName), inlet_(nullptr) { }
    virtual ~FSMStateProxy() {}

    void setInlet(const std::shared_ptr<OperationInletAPI>& inlet) { inlet_ = inlet; }


    virtual bool isActive() const override {
        return Value::boolValue(broker_->fsmState()->isActive(fsmFullName_, stateName_));
    }

    virtual bool activate() override {
        return Value::boolValue(broker_->fsmState()->activate(fsmFullName_, stateName_));
    }

    virtual bool deactivate() override {
        return Value::boolValue(broker_->fsmState()->deactivate(fsmFullName_, stateName_));
    }

    virtual bool isTransitable(const std::string& stateName) const override {
        return Value::boolValue(broker_->fsmState()->isTransitable(fsmFullName_, stateName_, stateName));
    }

    virtual Value bind(const std::shared_ptr<OperationAPI>& op)  override {
        auto opInfo = op->info();
        if (!opInfo.hasKey("broker")) {
            opInfo["broker"] = broker_->info();
        }
        return broker_->fsmState()->bindOperation(fsmFullName_, stateName_, opInfo);
    }

    virtual Value bind(const std::shared_ptr<OperationAPI>& op, const Value& arg)  override {
        auto opInfo = op->info();
        if (!opInfo.hasKey("broker")) {
            opInfo["broker"] = broker_->info();
        }
        return broker_->fsmState()->bindOperation(fsmFullName_, stateName_, opInfo, arg);
    }

    virtual Value bind(const std::shared_ptr<ECStateAPI>& ecs)  override {
        auto ecsInfo = ecs->info();
        if (!ecsInfo.hasKey("broker")) {
            ecsInfo["broker"] = broker_->info();
        }
        return broker_->fsmState()->bindECState(fsmFullName_, stateName_, ecsInfo);
    }

    virtual Value unbind(const std::shared_ptr<OperationAPI>& op)  override {
        auto opInfo = op->info();
        if (!opInfo.hasKey("broker")) {
            opInfo["broker"] = broker_->info();
        }
        return broker_->fsmState()->unbindOperation(fsmFullName_, stateName_, opInfo);
    }

    virtual Value unbind(const std::shared_ptr<ECStateAPI>& ecs)  override {
        auto ecsInfo = ecs->info();
        if (!ecsInfo.hasKey("broker")) {
            ecsInfo["broker"] = broker_->info();
        }
        return broker_->fsmState()->unbindECState(fsmFullName_, stateName_, ecsInfo);
    }

    virtual std::vector<std::shared_ptr<OperationAPI>> boundOperations()  override {
        auto infos = broker_->fsmState()->boundOperations(fsmFullName_, stateName_);
        return infos.const_list_map<std::shared_ptr<OperationAPI>>([this](auto opInfo) {
            return ProxyBuilder::operationProxy(opInfo, broker_);
        });
    }

    virtual std::vector<std::shared_ptr<ECStateAPI>> boundECStates()  override {
        // TODO: Not Implemented
        return {};
    }

    virtual std::shared_ptr<OperationInletAPI> inlet()  override {
        return inlet_;
    }

    virtual std::string ownerFullName() const { return fsmFullName_; }
};


std::string FSMStateInletProxy::name() const { return state_->fullName(); }

std::string FSMStateInletProxy::ownerFullName() const { return state_->ownerFullName(); }

Value FSMStateInletProxy::executeOwner() { 
    return state_->activate(); 
}

Value FSMStateInletProxy::info() const { return {{"fullName", state_->fullName()}}; }

std::vector<std::shared_ptr<ConnectionAPI>> FSMStateInletProxy::connections() const {
    return {};
}

Value FSMStateInletProxy::addConnection(const std::shared_ptr<ConnectionAPI>& c) {
    return Value::error(logger::error("FSMStateInletProxy::{}() not implemented.", __func__));
}  

Value FSMStateInletProxy::removeConnection(const std::string& _fullName)  {
    return {};
}



class FSMProxy : public FSMAPI {
private:
    const std::shared_ptr<BrokerProxyAPI> broker_;
    const std::string fullName_;

//    const std::shared_ptr<OperationOutletProxy> outlet_;

//    bool inlets_ready_;
 //   Value inletInfos_;
//    std::vector<std::shared_ptr<OperationInletAPI>> inlets_;
public:
    FSMProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName) : FSMAPI("FSMProxy", "Proxy", fullName), broker_(broker),
        fullName_(fullName) {}
    virtual ~FSMProxy() {}

public:

    virtual std::shared_ptr<FSMStateAPI> currentFsmState() const override {
        auto v = broker_->fsm()->currentFSMState(fullName_);
        return std::make_shared<FSMStateProxy>(broker_, fullName_, Value::string(v.at("name")));
        //return broker_->fsm()->currentFSMState(fullName_);
    }

    virtual std::shared_ptr<FSMStateAPI> fsmState(const std::string& stateName) const override {
        auto stateInfo = broker_->fsm()->fsmState(fullName_, stateName);
        if (stateInfo.isError()) return nullFSMState();
        auto state = std::make_shared<FSMStateProxy>(broker_, fullName_, stateName);
        auto inlet = std::make_shared<FSMStateInletProxy>(state);
        state->setInlet(inlet);
        return state;
    }

    virtual std::vector<std::shared_ptr<FSMStateAPI>> fsmStates() const override {
        //return broker_->fsm()->currentFSMState(fullName_);
    }
    
    virtual Value setFSMState(const std::string& stateName) override {
        //return broker_->fsm()->currentFSMState(fullName_);
    }

public:
/*
    virtual Value info() const override {
        return broker_->store()->getObjectInfo("operation", fullName_);
    }

    virtual Value fullInfo() const override {
        return broker_->operation()->fullInfo(fullName_);
    }

    virtual Value call(const Value& value) override {
        return broker_->operation()->call(fullName_, value);
    }

    virtual Value invoke() override  {
        return broker_->operation()->invoke(fullName_);
    }

    virtual Value execute() override {
        return broker_->operation()->execute(fullName_);
    }

    virtual std::shared_ptr<OperationOutletAPI> outlet() const override { return std::dynamic_pointer_cast<OperationOutletAPI>(outlet_); }
 
    virtual std::shared_ptr<OperationInletAPI> inlet(const std::string& name) const override {
      auto i = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(inlets(), [&name](auto i) { return i->name() == name; });
      if (i) return i.value();
      return nullOperationInlet();
    }
    
    virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override {
        if (inlets_ready_) {
            return inlets_;
        }
        return {};
    }
    */
};






///-------------- Operation Inlet




std::shared_ptr<FSMAPI> nerikiri::fsmProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName) {
    return std::make_shared<FSMProxy>(broker, fullName);
}