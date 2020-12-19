#include <nerikiri/operation_proxy.h>
#include <nerikiri/connection_proxy.h>
#include <nerikiri/functional.h>


using namespace nerikiri;
/// -------------- Operation Outlet


class OperationInletProxy: public OperationInletAPI {
private:
    const std::shared_ptr<BrokerProxyAPI> broker_;
    OperationAPI* owner_;
    const std::string fullName_;
    const std::string name_;
public:
    virtual ~OperationInletProxy() {}

    OperationInletProxy(OperationAPI* owner, const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName, const std::string& name) : owner_(owner), broker_(broker), fullName_(fullName), name_(name) {}

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
    virtual Value executeOwner() override { return owner_->execute(); }

    virtual Value info() const override {
        return broker_->operationInlet()->info(fullName_, name_);
    }

    virtual bool isUpdated() const override {
        return Value::boolValue(broker_->operationInlet()->isUpdated(fullName_, name_));
    }

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override {
        return broker_->operationInlet()->connections(fullName_, name_).const_list_map< std::shared_ptr<ConnectionAPI> >([this](auto v) {
            return connectionProxy(broker_, v);
        });
    }

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) override {
        return broker_->operationInlet()->defaultValue(fullName_, name_);
    }
    
    virtual Value removeConnection(const std::string& _fullName) override {
        return broker_->operationInlet()->defaultValue(fullName_, name_);
    }
 
};

class OperationOutletProxy : public OperationOutletAPI {
private:
    const std::shared_ptr<BrokerProxyAPI> broker_;
    OperationAPI* owner_;
    const std::string fullName_;
public:
    OperationOutletProxy(OperationAPI* owner, const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName) : owner_(owner), broker_(broker), fullName_(fullName) {}
    virtual ~OperationOutletProxy() {}
    
    // virtual OperationAPI* owner() override { return owner_; }

    virtual Value invokeOwner() override { return owner_->invoke(); }

    virtual std::string ownerFullName() const override { 
      return fullName_;
    }

    virtual Value get() const override {
        return broker_->operationOutlet()->get(fullName_);
    }


    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override {
        auto infos = broker_->operationOutlet()->connections(fullName_);
        if (infos.isError()) return {};

        
    }

    Value addConnection(const std::shared_ptr<ConnectionAPI>& c) override {
        auto value = broker_->operationOutlet()->addConnection(fullName_, c->info());
    }

    Value removeConnection(const std::string& _fullName) override {
        return broker_->operationOutlet()->removeConnection(fullName_, _fullName);
    }

    Value info() const override {
        return broker_->operationOutlet()->info(fullName_);
    }

};

class OperationProxy : public OperationAPI {
private:
    const std::shared_ptr<BrokerProxyAPI> broker_;
    const std::string fullName_;

    const std::shared_ptr<OperationOutletProxy> outlet_;

    bool inlets_ready_;
    Value inletInfos_;
    std::vector<std::shared_ptr<OperationInletAPI>> inlets_;
public:
    OperationProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName) : OperationAPI("OperationProxy", "Proxy", fullName), broker_(broker),
        fullName_(fullName), outlet_(std::make_shared<OperationOutletProxy>(this, broker, fullName)), inlets_ready_(false) {
        if (!inlets_ready_) {
            inletInfos_ = broker_->operation()->inlets(fullName_);
            if (inletInfos_.isError()) {
                return;
            }
            inlets_.clear();
            inletInfos_.const_list_for_each([this](auto inletInfo) {
                this->inlets_.push_back(std::make_shared<OperationInletProxy>(this, broker_, fullName_, Value::string(inletInfo.at("name"))));
            });
            inlets_ready_ = true;
        }
    }
    virtual ~OperationProxy() {}
public:
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
};






///-------------- Operation Inlet




std::shared_ptr<OperationAPI> nerikiri::operationProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName) {
    return std::make_shared<OperationProxy>(broker, fullName);
}