#include <juiz/connection_proxy.h>
#include <juiz/operation_api.h>

#include "../operation/operation_proxy.h"

using namespace juiz;


class ConnectionProxy : public ConnectionAPI {
private:
    const Value info_;
    std::shared_ptr<InletAPI> inletProxy_;
    const std::shared_ptr<ClientProxyAPI> broker_;
public:
    ConnectionProxy(const std::shared_ptr<ClientProxyAPI>& broker, const Value& info, const ConnectionType type) 
    : ConnectionAPI ("ConnectionProxy", "ConnectionProxy", type), broker_(broker), info_(info) {}
    virtual ~ConnectionProxy() {}
public:

    virtual Value info() const override {
        return info_;
    }

    virtual std::string fullName() const override {
        //logger::trace("ConnectionProxy::fullName() = {}", info_);
        return info_["fullName"].stringValue();
    }

    virtual std::shared_ptr<InletAPI> inlet() const override {
        // TODO: ここでinletの実体を返さないとdeleteできない．
        if (info_.at("inlet").hasKey("operation")) {
            return juiz::operationInletProxy(nullptr, broker_, Value::string(info_.at("inlet").at("operation").at("fullName")), Value::string(info_.at("inlet").at("name")));
        } else {
            return juiz::operationInletProxy(nullptr, broker_, Value::string(info_.at("inlet").at("ownerFullName")), Value::string(info_.at("inlet").at("name")));
        }
        return nullOperationInlet();
    }

    virtual std::shared_ptr<OutletAPI> outlet() const override {
        // TODO: ここでoutletの実体を返さないとdeleteできない．
        return juiz::operationOutletProxy(nullptr, broker_, Value::string(info_.at("outlet").at("ownerFullName")));

    }

    virtual Value pull() override {
        return Value::error(logger::error("ConnectionProxy::pull() is not implemented"));
    }

    virtual Value put(const Value& value) override {
        return Value::error(logger::error("ConnectionProxy::put() is not implemented"));

    }
};

std::shared_ptr<ConnectionAPI> juiz::connectionProxy(const std::shared_ptr<ClientProxyAPI>& broker, const Value& info) {
    auto typ = Value::string(info.at("type"));
    auto typeFlag = ConnectionAPI::ConnectionType::PULL;
    if (typ == "PULL") {}
    else if (typ == "PUSH") { typeFlag = ConnectionAPI::ConnectionType::PULL; }
    else if (typ == "EVENT") { typeFlag = ConnectionAPI::ConnectionType::EVENT; }

    return std::make_shared<ConnectionProxy>(broker, info, typeFlag);
}
