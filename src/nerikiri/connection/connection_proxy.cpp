#include <nerikiri/connection_proxy.h>
#include <nerikiri/operation_api.h>

#include "nerikiri/operation/operation_proxy.h"

using namespace nerikiri;


class ConnectionProxy : public ConnectionAPI {
private:
    const Value info_;
    std::shared_ptr<OperationInletAPI> inletProxy_;
    const std::shared_ptr<BrokerProxyAPI> broker_;
public:
    ConnectionProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const Value& info, const ConnectionType type) : ConnectionAPI ("ConnectionProxy", "ConnectionProxy", type), broker_(broker), info_(info) {}
    virtual ~ConnectionProxy() {}
public:

    virtual std::shared_ptr<OperationInletAPI> inlet() const {
        // TODO: ここでinletの実体を返さないとdeleteできない．
        if (info_.at("inlet").hasKey("operation")) {
            return nerikiri::operationInletProxy(nullptr, broker_, Value::string(info_.at("inlet").at("operation").at("fullName")), Value::string(info_.at("inlet").at("name")));
        }
        return nullOperationInlet();
    }

    virtual std::shared_ptr<OperationOutletAPI> outlet() const {
        // TODO: ここでoutletの実体を返さないとdeleteできない．
        return nerikiri::operationOutletProxy(nullptr, broker_, Value::string(info_.at("outlet").at("ownerFullName")));

    }

    virtual Value pull() {}

    virtual Value put(const Value& value) {}
};

std::shared_ptr<ConnectionAPI> nerikiri::connectionProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const Value& info) {
    auto typ = Value::string(info.at("type"));
    auto typeFlag = ConnectionAPI::ConnectionType::PULL;
    if (typ == "PULL") {}
    else if (typ == "PUSH") { typeFlag = ConnectionAPI::ConnectionType::PULL; }
    else if (typ == "EVENT") { typeFlag = ConnectionAPI::ConnectionType::EVENT; }

    return std::make_shared<ConnectionProxy>(broker, info, typeFlag);
}