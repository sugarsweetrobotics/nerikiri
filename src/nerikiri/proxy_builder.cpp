#include <nerikiri/proxy_builder.h>


#include <nerikiri/fsm_proxy.h>
#include <nerikiri/ec_proxy.h>
#include <nerikiri/operation_proxy.h>
#include <nerikiri/objectfactory.h>

using namespace nerikiri;



std::shared_ptr<OperationAPI> ProxyBuilder::operationProxy(const nerikiri::Value& value, nerikiri::ProcessStore* store) {
    auto fullName = Value::string(value.at("fullName"));
    std::string brokerTypeName = "CoreBroker";
    if (!value.hasKey("broker")) {
        return store->operation(fullName);
    }
    auto broker = store->brokerFactory(Value::string(value.at("broker").at("typeName")))->createProxy(value.at("broker"));
    auto op = nerikiri::operationProxy(broker, fullName);
    Value info = store->addOperationProxy(op);
    if (info.isError()) {
        return store->operationProxy(value);
    }
    return op;
}

/*
std::shared_ptr<FSMAPI> ProxyBuilder::fsmProxy(const nerikiri::Value& value, nerikiri::ProcessStore* store) {
    /* auto fullName = Value::string(value.at("fullName"));
    std::string brokerTypeName = "CoreBroker";
    if (!value.hasKey("broker")) {
        return store->fsm(fullName);
    }
    auto broker = store->brokerFactory(Value::string(value.at("broker").at("typeName")))->createProxy(value.at("broker"));
    auto fsm = nerikiri::fsmProxy(broker, fullName);
    Value info = store->addFSMProxy(fsm);
    if (info.isError()) {
        return store->fsmProxy(value);
    }
    return fsm; *
    return nullFSM();
}
*/



std::shared_ptr<OperationAPI> ProxyBuilder::operationProxy(const nerikiri::Value& value, const std::shared_ptr<BrokerProxyAPI>& broker) {
    return nerikiri::operationProxy(broker, Value::string(value.at("fullName")));
}


/*
std::shared_ptr<ECStateAPI> ProxyBuilder::ecStateProxy(const Value& value, ProcessStore* store) {
    auto fullName = Value::string(value.at("fullName"));
//    auto op = store->operation(Value::string(value.at("fullName")));
    auto broker = store->brokerFactory(Value::string(value.at("broker").at("typeName")))->createProxy(value.at("broker"));
    if (Value::string(value.at("stateName")) == "started") {
        return nerikiri::ecStateStartProxy(broker, fullName);
    } else if (Value::string(value.at("stateName")) == "stopped") {
        return nerikiri::ecStateStopProxy(broker, fullName);
    }
    //return nullECState();

    // TODO: ここでnullECを返すはず
}
*/

/*
std::shared_ptr<ECStateAPI> ProxyBuilder::ecStateProxy(const Value& value, const std::shared_ptr<BrokerProxyAPI>& brokerProxy) {
    return nerikiri::ecStateStartProxy(brokerProxy, Value::string(value.at("fullName")));

}
*/
//std::shared_ptr<FSMAPI> ProxyBuilder::fsmProxy(const nerikiri::Value& value, const std::shared_ptr<BrokerProxyAPI>& broker) {
    // return nerikiri::fsmProxy(broker, Value::string(value.at("fullName")));
//}



/**
 * inletOwnerClassName must be "operation" or "fsm"
 */
bool check_the_same_route_connection_exists(const std::vector<std::shared_ptr<ConnectionAPI>>& connections, const Value& conInfo, const std::string& inletOwnerClassName) {
    auto flag = false;
    nerikiri::functional::for_each<std::shared_ptr<ConnectionAPI>>(connections, [&flag, &conInfo, &inletOwnerClassName](auto con) {
        if ((con->inlet()->ownerFullName() == Value::string(conInfo.at("inlet").at(inletOwnerClassName).at("fullName"))) && 
            (con->inlet()->name() == Value::string(conInfo.at("inlet").at("name"))) &&
            (con->outlet()->ownerFullName() == Value::string(conInfo.at("outlet").at("operation").at("fullName")))) {
                flag = true;
            }
    });
    return flag;
}

bool check_the_same_name_connection_exists(const std::vector<std::shared_ptr<ConnectionAPI>>& connections, const std::string& name) {
    auto con = nerikiri::functional::find<std::shared_ptr<ConnectionAPI>>(connections, [&name](auto c) {
        return c->fullName() == name;
    });
    if (con) return true;
    return false;
}

std::string applyConnectionAutoRename(const std::string& name, const int count_hint=0) {
    return name;
}

std::shared_ptr<ConnectionAPI> ProxyBuilder::outgoingOperationConnectionProxy(const Value& value, ProcessStore* store) {

    auto outlet_side_operation = store->operation(Value::string(value.at("outlet").at("operation").at("fullName")));

    std::shared_ptr<OperationInletAPI> inlet = nullptr;
    std::shared_ptr<Object> inlet_holder = nullptr;
    auto className = "operation";
    if (value.at("inlet").hasKey("operation")) {
        // Check the same connection route is already connected
        auto inlet_side_operationProxy = ProxyBuilder::operationProxy(value.at("inlet").at("operation"), store);
        auto inlet_opt = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(inlet_side_operationProxy->inlets(), 
            [&value](auto i) { return i->name() == Value::string(value.at("inlet").at("name")); }
        );
        if (!inlet_opt) {
            logger::error("ProxyBuilder::{}({}) failed. Inlet not found", value);
            return nullConnection();
        }
        inlet = inlet_opt.value();
        inlet_holder = inlet_side_operationProxy;
    } /* else if (value.at("inlet").hasKey("fsm")) {
        className = "fsm";
        auto inlet_side_fsm_proxy = ProxyBuilder::fsmProxy(value.at("inlet").at("fsm"), store);
        inlet = inlet_side_fsm_proxy->fsmState(Value::string(value.at("inlet").at("name")))->inlet();
        inlet_holder = inlet_side_fsm_proxy;
      //return createConnection(Value::string(value.at("name")), connectionType(Value::string(value.at("type"))), inlet, outlet_side_operation->outlet(), inlet_side_fsm_proxy);
    }*/ else if (value.at("inlet").hasKey("topic")) {
        className = "topic";
        auto inlet_side_topic_info = ObjectFactory::createTopic(*store, value.at("inlet").at("topic"));
        auto inlet_side_topic = store->topic(Value::string(value.at("inlet").at("topic").at("fullName")));
        inlet = inlet_side_topic->inlet(Value::string(value.at("inlet").at("name")));
        inlet_holder = inlet_side_topic;
    }

    if (check_the_same_route_connection_exists(outlet_side_operation->outlet()->connections(), value, className)) {
        logger::error("ProxyBuilder::{}({}) fails. Outlet side has the same route connection", __func__, value);
        return nullConnection();
    }


    auto name = Value::string(value.at("name"));
    int count_hint = 0;
    while (check_the_same_name_connection_exists(outlet_side_operation->outlet()->connections(), name)) {
        logger::error("ProxyBuilder::{}({}) fails. Outlet side has the same name connection", __func__, value);
        // TODO: ここでConnectionInfoがオート裏ネームの設定ならリネームする
        if (value.hasKey("namingPolicy")) {
            auto v = Value::string(value.at("namingPolicy"));
            if (v == "auto") {
                name = applyConnectionAutoRename(name, count_hint++);
            }
        } else { break; }
    }
    //if (check_the_same_route_connection_exists(inlet.value()->connections(), value, "operation")) {
    //    logger::error("ProxyBuilder::{}({}) fails. Inlet side has the same name connection", __func__, value);
    //    return nullConnection();
    // }
    return createConnection(name, connectionType(Value::string(value.at("type"))), inlet, outlet_side_operation->outlet(), inlet_holder);
}

std::shared_ptr<ConnectionAPI> ProxyBuilder::incomingOperationConnectionProxy(const Value& value, ProcessStore* store) {
    if (value.at("inlet").hasKey("operation")) {
        auto outlet_side_operationProxy = ProxyBuilder::operationProxy(value.at("outlet").at("operation"), store);
        auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(
            store->operation(Value::string(value.at("inlet").at("operation").at("fullName")))->inlets(), 
            [&value](auto i) { return i->name() == Value::string(value.at("inlet").at("name")); }
        );
        if (!inlet) {
            return nullConnection();
        }

        //if (check_the_same_route_connection_exists(outlet_side_operationProxy->outlet()->connections(), value, "operation")) {
        //    logger::error("ProxyBuilder::{}({}) fails. Outlet side has the same name connection", __func__, value);
        //    return nullConnection();
        //}
        if (check_the_same_route_connection_exists(inlet.value()->connections(), value, "operation")) {
            logger::error("ProxyBuilder::{}({}) fails. Inlet side has the same route connection", __func__, value);
            return nullConnection();
        }
        auto name = Value::string(value.at("name"));
        int count_hint = 0;
        while (check_the_same_name_connection_exists(inlet.value()->connections(), name)) {
            logger::error("ProxyBuilder::{}({}) fails. Outlet side has the same name connection", __func__, value);
            // TODO: ここでConnectionInfoがオート裏ネームの設定ならリネームする
            if (value.hasKey("namingPolicy")) {
                auto v = Value::string(value.at("namingPolicy"));
                if (v == "auto") {
                    name = applyConnectionAutoRename(name, count_hint++);
                }
            } else {
                break;
            }
        }
        return createConnection(name, connectionType(Value::string(value.at("type"))), inlet.value(), outlet_side_operationProxy->outlet(), outlet_side_operationProxy);
    } /* else if (value.at("inlet").hasKey("fsm")) {
        auto op = ProxyBuilder::operationProxy(value.at("outlet").at("operation"), store);
        auto inlet = store->fsm(Value::string(value.at("inlet").at("fsm").at("fullName")))->fsmState(Value::string(value.at("inlet").at("name")))->inlet();
        return createConnection(Value::string(value.at("name")), connectionType(Value::string(value.at("type"))), inlet, op->outlet(), op);
   } */

    logger::error("ProxyBuilder::outgoingOperationConnectionProxy({}) failed. Top level 'outlet' object must have 'operation' element object.", value);
    return nullConnection();
}
