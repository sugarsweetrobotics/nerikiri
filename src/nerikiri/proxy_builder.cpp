#include <nerikiri/proxy_builder.h>


#include <nerikiri/fsm_proxy.h>
#include <nerikiri/ec_proxy.h>
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

std::shared_ptr<ECStateAPI> ProxyBuilder::ecStateProxy(const Value& value, ProcessStore* store) {
    auto fullName = Value::string(value.at("fullName"));
//    auto op = store->operation(Value::string(value.at("fullName")));
    auto broker = store->brokerFactory(Value::string(value.at("broker").at("typeName")))->createProxy(value.at("broker"));
    if (Value::string(value.at("stateName")) == "started") {
        return nerikiri::ecStateStartProxy(broker, fullName);
    } else if (Value::string(value.at("stateName")) == "stopped") {
        return nerikiri::ecStateStopProxy(broker, fullName);
    }
    return nullECState();
}

std::shared_ptr<ECStateAPI> ProxyBuilder::ecStateProxy(const Value& value, const std::shared_ptr<BrokerProxyAPI>& brokerProxy) {
    return nerikiri::ecStateStartProxy(brokerProxy, Value::string(value.at("fullName")));

}

std::shared_ptr<FSMAPI> ProxyBuilder::fsmProxy(const nerikiri::Value& value, const std::shared_ptr<BrokerProxyAPI>& broker) {
    return nerikiri::fsmProxy(broker, Value::string(value.at("fullName")));
}

std::shared_ptr<FSMAPI> ProxyBuilder::fsmProxy(const nerikiri::Value& value, nerikiri::ProcessStore* store) {
    auto fullName = Value::string(value.at("fullName"));
    auto brk = store->brokerFactory(Value::string(value.at("broker").at("typeName")))->createProxy(value.at("broker"));
    return nerikiri::fsmProxy(brk, fullName);
}

std::shared_ptr<ConnectionAPI> ProxyBuilder::outgoingOperationConnectionProxy(const Value& value, ProcessStore* store) {
    if (value.at("inlet").hasKey("operation")) {
        auto opp = ProxyBuilder::operationProxy(value.at("inlet").at("operation"), store);
        auto op = store->operation(Value::string(value.at("outlet").at("operation").at("fullName")));
        auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(opp->inlets(), 
            [&value](auto i) { return i->name() == Value::string(value.at("inlet").at("name")); }
        );
        if (inlet) return createConnection(Value::string(value.at("name")), connectionType(Value::string(value.at("type"))), inlet.value(), op->outlet());
        return nullConnection();
    } else if (value.at("inlet").hasKey("fsm")) {
        auto fsmp = ProxyBuilder::fsmProxy(value.at("inlet").at("fsm"), store);
        auto op = store->operation(Value::string(value.at("outlet").at("operation").at("fullName")));
        auto inlet = fsmp->fsmState(Value::string(value.at("inlet").at("name")))->inlet();
        return createConnection(Value::string(value.at("name")), connectionType(Value::string(value.at("type"))), inlet, op->outlet());
    }

    logger::error("ProxyBuilder::outgoingOperationConnectionProxy({}) failed. 'inlet' object must have 'operation' or 'fsm' element object.", value);
    return nullConnection();
}

std::shared_ptr<ConnectionAPI> ProxyBuilder::incomingOperationConnectionProxy(const Value& value, ProcessStore* store) {
    if (value.at("inlet").hasKey("operation")) {
        auto op = ProxyBuilder::operationProxy(value.at("outlet").at("operation"), store);
        auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(
            store->operation(Value::string(value.at("inlet").at("operation").at("fullName")))->inlets(), 
            [&value](auto i) { return i->name() == Value::string(value.at("inlet").at("name")); }
        );
        if (!inlet) {
            return nullConnection();
        }
        return createConnection(Value::string(value.at("name")), connectionType(Value::string(value.at("type"))), inlet.value(), op->outlet());
    } else if (value.at("inlet").hasKey("fsm")) {
        auto op = ProxyBuilder::operationProxy(value.at("outlet").at("operation"), store);
        auto inlet = store->fsm(Value::string(value.at("inlet").at("fsm").at("fullName")))->fsmState(Value::string(value.at("inlet").at("name")))->inlet();
        return createConnection(Value::string(value.at("name")), connectionType(Value::string(value.at("type"))), inlet, op->outlet());
   }

    logger::error("ProxyBuilder::outgoingOperationConnectionProxy({}) failed. Top level 'outlet' object must have 'operation' element object.", value);
    return nullConnection();
}