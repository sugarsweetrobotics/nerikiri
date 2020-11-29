#include <nerikiri/proxy_builder.h>


#include <nerikiri/operation_proxy.h>
using namespace nerikiri;

std::shared_ptr<OperationAPI> ProxyBuilder::operationProxy(const nerikiri::Value& value, const std::shared_ptr<BrokerProxyAPI>& broker) {
    return nerikiri::operationProxy(broker, Value::string(value.at("fullName")));
}

std::shared_ptr<OperationAPI> ProxyBuilder::operationProxy(const nerikiri::Value& value, nerikiri::ProcessStore* store) {
    auto fullName = Value::string(value.at("fullName"));
//    auto op = store->operation(Value::string(value.at("fullName")));
    auto brk = store->brokerFactory(Value::string(value.at("broker").at("typeName")))->createProxy(value.at("broker"));
    return nerikiri::operationProxy(brk, fullName);
}


std::shared_ptr<ConnectionAPI> ProxyBuilder::outgoingConnectionProxy(const Value& value, ProcessStore* store) {
    auto opp = ProxyBuilder::operationProxy(value.at("inlet").at("operation"), store);
    auto op = store->operation(Value::string(value.at("outlet").at("operation").at("fullName")));
    auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(opp->inlets(), 
        [&value](auto i) { return i->name() == Value::string(value.at("inlet").at("name")); }
    );
    if (inlet) return createConnection(Value::string(value.at("name")), connectionType(Value::string(value.at("type"))), inlet.value(), op->outlet());
    return nullConnection();
}

std::shared_ptr<ConnectionAPI> ProxyBuilder::incomingConnectionProxy(const Value& value, ProcessStore* store) {
    auto op = ProxyBuilder::operationProxy(value.at("outlet").at("operation"), store);
    auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(
        store->operation(Value::string(value.at("inlet").at("operation").at("fullName")))->inlets(), 
        [&value](auto i) { return i->name() == Value::string(value.at("inlet").at("name")); }
    );
    return createConnection(Value::string(value.at("name")), connectionType(Value::string(value.at("type"))), inlet.value(), op->outlet());
}