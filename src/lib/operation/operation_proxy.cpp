#include <juiz/operation.h>
#include <juiz/connection_proxy.h>
#include <juiz/utils/functional.h>

#include "operation_proxy.h"

using namespace juiz;
/// -------------- Operation Outlet

class OperationProxy;

class OperationInletProxy: public InletAPI {
private:
    const std::shared_ptr<ClientProxyAPI> broker_;
    OperationAPI* owner_;
    std::shared_ptr<OperationProxy> ownerProxy_;
    const std::string fullName_;
    const std::string name_;
public:
    virtual ~OperationInletProxy() {}

    OperationInletProxy(OperationAPI* owner, const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName, const std::string& name) : 
      owner_(owner), broker_(broker), fullName_(fullName), name_(name), ownerProxy_(nullptr) {}

    virtual bool isNull() const override {
        return false;
    }
    
    virtual std::string name() const override {
        return name_;
    }

    virtual std::string ownerFullName() const override { 
      return fullName_;
    }

    virtual Value defaultValue() const override {
        return broker_->operationInlet()->defaultValue(fullName_, name_);
    }

    virtual Value get() const override {
        return broker_->operationInlet()->get(fullName_, name_);
    }

    virtual Value put(const Value& value) override {
        return broker_->operationInlet()->put(fullName_, name_, value);
    }

    /// virtual OperationAPI* owner() override { return owner_; }
    virtual Value executeOwner() override ;
    /*
    virtual Value executeOwner() override {
        if (!ownerProxy_) {
            // ここでproxyを実体化しておく
            ownerProxy_ = std::make_shared<OperationProxy>(broker_, fullName_);
        }
        //return owner_->execute(); 
        return ownerProxy_->execute();
    }
    */

    virtual Value fullInfo() const override {
        auto i = broker_->operationInlet()->info(fullName_, name_);
        i["broker"] = broker_->fullInfo();
        return i;
    }

    virtual Value info() const override {
        auto i = broker_->operationInlet()->info(fullName_, name_);
        i["broker"] = broker_->fullInfo();
        return i;
    }

    virtual bool isUpdated() const override {
        return Value::boolValue(broker_->operationInlet()->isUpdated(fullName_, name_));
    }

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override {
        return broker_->operationInlet()->connections(fullName_, name_).const_list_map< std::shared_ptr<ConnectionAPI> >([this](auto v) {
            return connectionProxy(broker_, v);
        });
    }

    //virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) override {
    //    return broker_->operationInlet()->defaultValue(fullName_, name_);
    //}


    virtual Value connectTo(const std::shared_ptr<OutletAPI>& outlet, const Value& connectionInfo) override {
        // TODO: connect imple
        return broker_->operationInlet()->connectTo(fullName_, name_, connectionInfo);
    }


    virtual Value disconnectFrom(const std::shared_ptr<OutletAPI>& outlet) override {
        // TODO: connect imple
    //    return broker_->operationInlet()->defaultValue(fullName_, name_);
    }

    
    //virtual Value removeConnection(const std::string& _fullName) override {
    //    return broker_->operationInlet()->defaultValue(fullName_, name_);
    //}
 
};


std::shared_ptr<InletAPI> juiz::operationInletProxy(OperationAPI* owner, const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName, const std::string& name) {
    return std::make_shared<OperationInletProxy>(owner, broker, fullName, name);
} 



class OperationOutletProxy : public OutletAPI {
private:
    const std::shared_ptr<ClientProxyAPI> broker_;
    OperationAPI* owner_;
    const std::string fullName_;
    std::shared_ptr<OperationAPI> ownerProxy_;
public:
    OperationOutletProxy(OperationAPI* owner, const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName) : owner_(owner), broker_(broker), fullName_(fullName), ownerProxy_(nullptr) {}
    virtual ~OperationOutletProxy() {}


    virtual Value invokeOwner() override { 
        return owner_->invoke(); 
    }

    virtual std::string ownerFullName() const override { 
      return fullName_;
    }

    virtual Value get() const override {
        return broker_->operationOutlet()->get(fullName_);
    }


    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override {
        auto infos = broker_->operationOutlet()->connections(fullName_);
        if (infos.isError()) return {};
        return infos.const_list_map<std::shared_ptr<ConnectionAPI>>([this](auto info) {
            return connectionProxy(broker_, info);
        });
    }

    /*
    Value addConnection(const std::shared_ptr<ConnectionAPI>& c) override {
    //    auto value = broker_->operationOutlet()->addConnection(fullName_, c->info());
    }
    */


    virtual Value connectTo(const std::shared_ptr<InletAPI>& inlet, const Value& connectionInfo) override {
        // TODO: connect imple
        return broker_->operationOutlet()->connectTo(fullName_, connectionInfo);
    }

    virtual Value disconnectFrom(const std::shared_ptr<InletAPI>& inlet) override {
        // TODO: connect imple
        return broker_->operationOutlet()->disconnectFrom(fullName_, inlet->info());
    }

    /*
    Value removeConnection(const std::string& _fullName) override {
    //    return broker_->operationOutlet()->removeConnection(fullName_, _fullName);
    }
    */

    Value info() const override {
        auto i = broker_->operationOutlet()->info(fullName_);
        i["broker"] = broker_->fullInfo();
        return i;
    }

};


std::shared_ptr<OutletAPI> juiz::operationOutletProxy(OperationAPI* owner, const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName) {
    return std::make_shared<OperationOutletProxy>(owner, broker, fullName);
} 


class OperationProxy : public OperationAPI {
private:
    const std::shared_ptr<ClientProxyAPI> broker_;
    const std::string fullName_;

    const std::shared_ptr<OperationOutletProxy> outlet_;

    bool inlets_ready_;
    Value inletInfos_;
    std::shared_ptr<InletAPI> event_inlet_;
    std::shared_ptr<InletAPI> argument_inlet_;
    std::vector<std::shared_ptr<InletAPI>> inlets_;
public:
    OperationProxy(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName) : OperationAPI("OperationProxy", "Proxy", fullName), broker_(broker),
        fullName_(fullName), outlet_(std::make_shared<OperationOutletProxy>(this, broker, fullName)), inlets_ready_(false) {
        inletInfos_ = broker_->operation()->inlets(fullName_);
        if (inletInfos_.isError()) {
            this->_setNull();
            return;
        }
        inlets_.clear();
        inletInfos_.const_list_for_each([this](auto inletInfo) {
            this->inlets_.push_back(std::make_shared<OperationInletProxy>(this, broker_, fullName_, Value::string(inletInfo.at("name"))));
        });
        event_inlet_ = std::make_shared<OperationInletProxy>(this, broker_, fullName_, "__event__");
        argument_inlet_ = std::make_shared<OperationInletProxy>(this, broker_, fullName_, "__argument__");
        inlets_ready_ = true;
    }

    virtual ~OperationProxy() {
        inlets_.clear();
        
    }

        
    virtual Value brokerInfo() const override {
        return broker_->info();
    }
    
public:
    virtual Value info() const override {
        logger::trace("OperationProxy::info() called");
        auto i = broker_->store()->getObjectInfo("operation", fullName_);
        i["broker"] = broker_->fullInfo();
        logger::trace("OperationProxy::info() exited");
        return i;
    }

    virtual Value fullInfo() const override {
        auto i = broker_->operation()->fullInfo(fullName_);
        if (i.isError()) return i;
        i["broker"] = broker_->fullInfo();
        return i;
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

    virtual std::shared_ptr<OutletAPI> outlet() const override {
         return std::dynamic_pointer_cast<OutletAPI>(outlet_);
    }
 
    virtual std::shared_ptr<InletAPI> inlet(const std::string& name) const override {
        if (name == "__event__") { 
            return event_inlet_;
        }
        else if (name == "__argument__") { 
            return argument_inlet_;
        }
      auto i = juiz::functional::find<std::shared_ptr<InletAPI>>(inlets(), [&name](auto i) { return i->name() == name; });
      if (i) return i.value();
      return nullOperationInlet();
    }
    
    virtual std::vector<std::shared_ptr<InletAPI>> inlets() const override {
        if (inlets_ready_) {
            return inlets_;
        }
        return {};
    }
};




Value OperationInletProxy::executeOwner() {
        if (!ownerProxy_) {
            // ここでproxyを実体化しておく
            ownerProxy_ = std::make_shared<OperationProxy>(broker_, fullName_);
        }
        //return owner_->execute(); 
        return ownerProxy_->execute();
    }

///-------------- Operation Inlet

namespace juiz {
    std::shared_ptr<OperationAPI> operationProxy(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName);

}

std::shared_ptr<OperationAPI> juiz::operationProxy(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullName) {
    logger::trace("juiz::operationProxy(broker='{}', fullName='{}') called.", broker->typeName(), fullName);
    return std::make_shared<OperationProxy>(broker, fullName);
}
