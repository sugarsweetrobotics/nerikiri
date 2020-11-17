#pragma once

#include <nerikiri/operation_api.h>
#include <nerikiri/broker_proxy_api.h>

#include <nerikiri/operation_inlet_api.h>
#include <nerikiri/operation_outlet_api.h>


namespace nerikiri {
    class OperationInletProxy: public OperationInletAPI {
    private:
        const std::shared_ptr<BrokerProxyAPI> broker_;
        OperationAPI* owner_;
        const std::string fullName_;
        const std::string name_;
    public:
        OperationInletProxy(OperationAPI* owner, const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName, const std::string& name);
        virtual ~OperationInletProxy();

        virtual std::string name() const override {
           return name_;
        }

        virtual Value defaultValue() const override;
        
        virtual Value get() const override;

        virtual Value put(const Value& value) override;

        virtual OperationAPI* owner() override { return owner_; }

        virtual Value info() const override;

        virtual bool isUpdated() const override;

        virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override;

        virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) override;
        
        virtual Value removeConnection(const std::string& _fullName) override;
    };

    class OperationOutletProxy : public OperationOutletAPI {
    private:
        const std::shared_ptr<BrokerProxyAPI> broker_;
        OperationAPI* owner_;
        const std::string fullName_;
    public:
        OperationOutletProxy(OperationAPI* owner, const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName);
        virtual ~OperationOutletProxy();

        virtual OperationAPI* owner() override { return owner_; }

        virtual Value get() const override;

        virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override;

        virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) override;

        virtual Value removeConnection(const std::string& _fullName) override;

        virtual Value info() const override;

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

        virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override {
            if (inlets_ready_) {
                return inlets_;
            }
            return {};
        }
    };

}