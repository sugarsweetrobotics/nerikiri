#include <nerikiri/connection_proxy.h>

using namespace nerikiri;


class ConnectionProxy : public ConnectionAPI {

public:
    ConnectionProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const Value& info, const ConnectionType type) : ConnectionAPI ("ConnectionProxy", "ConnectionProxy", type) {

    }
    virtual ~ConnectionProxy() {}
public:

    virtual std::shared_ptr<OperationInletAPI> inlet() const {}

    virtual std::shared_ptr<OperationOutletAPI> outlet() const {}

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