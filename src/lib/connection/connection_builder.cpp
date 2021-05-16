#include <sstream>

#include <juiz/nerikiri.h>
#include <juiz/logger.h>
#include "../objectfactory.h"


#include "../proxy_builder.h"
#include "../connection/connection_builder.h"


using namespace juiz;
/**
 * inletOwnerClassName must be "operation" or "fsm"
 */
static bool check_the_same_route_connection_exists(const std::shared_ptr<OutletAPI>& outlet, const std::shared_ptr<InletAPI>& inlet) {
    auto flag = false;
    juiz::functional::for_each<std::shared_ptr<ConnectionAPI>>(outlet->connections(), [&flag, &inlet](auto con) {
        if ((con->inlet()->ownerFullName() == Value::string(inlet->info().at("ownerFullName"))) && 
            (con->inlet()->name() == Value::string(inlet->info().at("name")))) {
          flag = true;
        }
    });
    return flag;
}

static bool check_the_same_name_connection_exists(const std::vector<std::shared_ptr<ConnectionAPI>>& connections, const std::string& name) {
    auto con = juiz::functional::find<std::shared_ptr<ConnectionAPI>>(connections, [&name](auto c) {
        return c->fullName() == name;
    });
    if (con) return true;
    return false;
}




static std::string applyConnectionAutoRename(const std::string& name, const int count_hint=0) {
  std::stringstream ss;
  ss << name << count_hint;
  return ss.str();
}


static std::string renameConnectionName(const std::vector<std::shared_ptr<ConnectionAPI>>& connections, const std::string& name) {
  auto _name = name;
  int count_hint = 0;
  while (check_the_same_name_connection_exists(connections, _name)) {
    std::stringstream ss;
    ss << name << count_hint++;
    _name = ss.str();
  }
  return _name;
}

Value ConnectionBuilder::connect(ProcessStore& store, const std::shared_ptr<OutletAPI>& outlet, const std::shared_ptr<InletAPI>& inlet, const Value& connectionInfo) {
  logger::trace("ConnectionBuilder::connect({}, {}, {}) called", outlet->info(), inlet->info(), connectionInfo);

  if (check_the_same_route_connection_exists(outlet, inlet)) {
    return Value::error(logger::error("ConnectionBuilder::{}({}) fails. Outlet side has the same route connection", __func__, connectionInfo));
  }
  auto name = Value::string(connectionInfo.at("name"));
  if (Value::string(connectionInfo.at("namingPolicy")) == "auto") {
    name = renameConnectionName(juiz::functional::join(outlet->connections(), inlet->connections()), name);
  }



}




Value ConnectionBuilder::createInletConnection(ProcessStore& store, const Value& connectionInfo, ClientProxyAPI* inletBroker) {
  logger::trace("ConnectionBuilder::createInletConnection(connectionInfo={})", connectionInfo);
  auto value = connectionInfo.at("inlet").at("operation");
  auto outlet = store.outletProxy(connectionInfo["outlet"]);
  return store.get<OperationAPI>(Value::string(value["fullName"]))->inlet(Value::string(connectionInfo.at("inlet").at("name")))->connectTo(outlet, connectionInfo);
  //return inletBroker->operationInlet()->addConnection(Value::string(value.at("fullName")), Value::string(connectionInfo.at("inlet").at("name")), connectionInfo);
}


Value ConnectionBuilder::createOutletConnection(ProcessStore& store, const Value& connectionInfo, ClientProxyAPI* outletBroker/*=nullptr*/) {
  logger::trace("ConnectionBuilder::createOutletConnection(connectionInfo={})", connectionInfo);
  auto value = connectionInfo.at("outlet").at("operation");
  auto inlet = store.inletProxy(connectionInfo["inlet"]);
  // outlet側から接続を構築する．失敗したらエラーを返す
  return store.get<OperationAPI>(Value::string(value["fullName"]))->outlet()->connectTo(inlet, connectionInfo);
  //if (outletConnectionResult.isError()) return outletConnectionResult;
}

/**
 * 接続の作成
 */
Value ConnectionBuilder::createOperationConnection(ProcessStore& store, const Value& connectionInfo_, BrokerAPI* receiverBroker/*=nullptr*/) {
  logger::trace("ConnectionBuilder::createOperationConnection() called");
  Value connectionInfo = connectionInfo_;
  //logger::trace("ConnectionBuilder::createConnection({}) called", connectionInfo);
  auto inlet = store.inletProxy(connectionInfo["inlet"]);
  auto outlet = store.outletProxy(connectionInfo["outlet"]);
  // 同一ルートがあるかどうか確認．あるならエラー
  if (check_the_same_route_connection_exists(outlet, inlet)) {
    return Value::error(logger::error("ProxyBuilder::{}({}) fails. Inlet side has the same name connection", __func__, connectionInfo));
  }

  // 同一名称があるかどうか確認．ある場合はnamingPolicyがautoならば名前自動更新
  auto name = Value::string(connectionInfo.at("name"));
  auto all_connections = juiz::functional::join(outlet->connections(), inlet->connections());
  if (check_the_same_name_connection_exists(all_connections, name)) {
    if (Value::string(connectionInfo.at("namingPolicy")) == "auto") {
      name = renameConnectionName(all_connections, name);
    } else {
      // 同一名称があってnamingPolicyがautoでない場合はエラーを返す
      return Value::error(logger::error("ConnectionBuilder::{}({}) failed. Outlet side has the same name connection", __func__, connectionInfo));
    }
  }
  connectionInfo["name"] = name;

  // outlet側から接続を構築する．失敗したらエラーを返す
  auto outletConnectionResult = outlet->connectTo(inlet, connectionInfo);
  if (outletConnectionResult.isError()) return outletConnectionResult;
  // inlet側から接続を構築．
  auto inletConnectionResult = inlet->connectTo(outlet, connectionInfo);
  if (inletConnectionResult.isError()) {
    // inlet側接続が失敗した場合，outlet側は成功しているのでoutlet側接続を削除する必要がある．
    logger::error("ConnectionBuilder::createConnection({}) failed. Outlet connection success but Inlet failed (Message: {}). Now trying to remove connection of outlet side.", inletConnectionResult);
    auto outletDisconnectionResult = outlet->disconnectFrom(inlet);
    if (outletDisconnectionResult.isError()) {
      logger::error("ConnectionBuilder::createConnection({}) failed. Outlet connection success but Inlet failed. Outlet side connection deletion failed. Error({})", outletDisconnectionResult);
      return outletDisconnectionResult;
    }
    return inletConnectionResult;
  }
  logger::trace("ConnectionBuilder::createOperationConnection() exit");
  return inletConnectionResult;
}


//Value ConnectionBuilder::createOperationToOperationConnection(ProcessStore& store, const Value& connectionInfo, BrokerAPI* receiverBroker/*=nullptr*/) {



//}


//Value ConnectionBuilder::createFSMStateToOperationConnection(ProcessStore& store, const Value& connectionInfo, BrokerAPI* receiverBroker/*=nullptr*/) {


  
//}

/*
Value ConnectionBuilder::createConnection(ProcessStore* store, const Value& connectionInfo, BrokerAPI* receiverBroker) {
  //auto outlet = ProxyBuilder::operationProxy(connectionInfo.at("outlet").at("operation"), store)->outlet();
  auto value = connectionInfo.at("inlet").at("operation");
  // TODO: brokerの項目がなかったらどうするか？
  auto inletBroker = store->brokerFactory(Value::string(value.at("broker").at("typeName")))->createProxy(value.at("broker"));
  auto ret0 = inletBroker->operationInlet()->addConnection(Value::string(value.at("fullName")), Value::string(connectionInfo.at("inlet").at("name")), connectionInfo);
  if (ret0.isError()) return ret0;
  // TODO: 名前が同じのがあったらどうするか？
  auto value2 = connectionInfo.at("outlet").at("operation");
  auto outletBroker = store->brokerFactory(Value::string(value2.at("broker").at("typeName")))->createProxy(value2.at("broker"));
  auto ret1 = outletBroker->operationOutlet()->addConnection(Value::string(value2.at("fullName")), connectionInfo);
  if (ret1.isError()) {
    inletBroker->operationInlet()->removeConnection(Value::string(value.at("fullName")), Value::string(connectionInfo.at("inlet").at("name")), Value::string(ret0.at("fullName")));
  }
  return ret1;
}
*/

//Value ConnectionBuilder::createStateBind(ProcessStore& store, const Value& connectionInfo_, BrokerAPI* receiverBroker/*=nullptr*/) {
    // これinlet側はFSM、outlet側はoperationを使う
    
//  Value connectionInfo = connectionInfo_;
  /*
  //auto outlet = ProxyBuilder::operationProxy(connectionInfo.at("outlet").at("operation"), store)->outlet();
  auto value = connectionInfo.at("inlet").at("fsm");
  auto inletBroker = store.brokerFactory(Value::string(value.at("broker").at("typeName")))->createProxy(value.at("broker"));

    
    auto inlet = store.fsmProxy(connectionInfo["inlet"]["fsm"])->fsmState(Value::string(connectionInfo["inlet"]["name"]))->inlet();
    auto outlet = store.operationProxy(connectionInfo["outlet"]["operation"])->outlet();
    //if ( outlet->isNull() || inlet->isNull() ) {
    //  return Value::error(logger::error("ConnectionBuilder::createConnection({}) error. Unavailable outlet-inlet pair.", connectionInfo));
    //}
  */

 // auto inlet = store.inletProxy(connectionInfo["inlet"]);
 // auto outlet = store.outletProxy(connectionInfo["outlet"]);
  // 同一ルートがあるかどうか確認．あるならエラー
 // if (check_the_same_route_connection_exists(outlet, inlet)) {
 //   return Value::error(logger::error("ProxyBuilder::{}({}) fails. Inlet side has the same name connection", __func__, connectionInfo));
 // }

  /*
  // TODO: 名前が同じのがあったらどうするか？
  auto cons = inletBroker->fsmStateInlet()->connections(Value::string(value.at("fullName")), Value::string(connectionInfo.at("inlet").at("name")));
  cons.const_list_for_each([&connectionInfo](const auto& c) {
    if (c.at("fullName").stringValue() == Value::string(connectionInfo.at("name"))) {
      logger::warn("ConnectionBuilder::createStateBind() warning. Same name connection is encountered");
      connectionInfo["name"] = connectionInfo["name"].stringValue() + "1";
    }
  });
  */


  // outlet側から接続を構築する．失敗したらエラーを返す
 // auto outletConnectionResult = outlet->connectTo(inlet, connectionInfo);
 // if (outletConnectionResult.isError()) return outletConnectionResult;
  // inlet側から接続を構築．
 // auto inletConnectionResult = inlet->connectTo(outlet, connectionInfo);
 // if (inletConnectionResult.isError()) {
    // inlet側接続が失敗した場合，outlet側は成功しているのでoutlet側接続を削除する必要がある．
 //   logger::error("ConnectionBuilder::createConnection({}) failed. Outlet connection success but Inlet failed (Message: {}). Now trying to remove connection of outlet side.", inletConnectionResult);
 //   auto outletDisconnectionResult = outlet->disconnectFrom(inlet);
 //   if (outletDisconnectionResult.isError()) {
 //     logger::error("ConnectionBuilder::createConnection({}) failed. Outlet connection success but Inlet failed. Outlet side connection deletion failed. Error({})", outletDisconnectionResult);
 //     return outletDisconnectionResult;
 //   }
 //   return inletConnectionResult;
 // }
 // return inletConnectionResult;


 /*
  inletBroker->fsmStateInlet()->connectTo(Value::string(value.at("fullName")), Value::string(connectionInfo.at("inlet").at("name")), connectionInfo);
  //inletBroker->fsmStateInlet()->addConnection(Value::string(value.at("fullName")), Value::string(connectionInfo.at("inlet").at("name")), connectionInfo);
  // TODO: 名前が同じのがあったらどうするか？
  value = connectionInfo.at("outlet").at("operation");
  auto outletBroker = store.brokerFactory(Value::string(value.at("broker").at("typeName")))->createProxy(value.at("broker"));
  // TODO: ここはconnectToに変更した方がいいな
    return outletBroker->operationOutlet()->connectTo(Value::string(value.at("fullName")), connectionInfo);
  */
    // return outletBroker->operationOutlet()->addConnection(Value::string(value.at("fullName")), connectionInfo);
//}

/*
Value ConnectionBuilder::registerTopicPublisher(ProcessStore& store, const Value& cInfo, const Value& opInfo, const Value& topicInfo) {
  logger::trace("ConnectionBuilder::registerTopicPublisher({}, {}, {})", cInfo, opInfo, topicInfo);
  auto op = store.get<OperationAPI>(juiz::naming::join(Value::string(cInfo.at("fullName")), Value::string(opInfo.at("fullName"))));
  auto topicInfo2 = ObjectFactory::createTopic(store, topicInfo);
  auto connectionName = "topic_connection_" + Value::string(topicInfo.at("fullName")) + "_" + op->fullName();
  Value connectionInfo {
    {"name", connectionName},
    {"type", "event"},
    {"inlet", {
      {"name", "data"},
      {"broker", {
        {"typeName", "CoreBroker"}
      }},
      {"topic", 
        topicInfo
      }
    }},
    {"outlet", {
      {"operation", opInfo}
    }}
  };
  return op->outlet()->addConnection(ProxyBuilder::outgoingOperationConnectionProxy(connectionInfo, &store));
}
*/
  /*
Value ConnectionBuilder::registerTopicPublisher(ProcessStore& store, const Value& opInfo, const Value& topicInfo) {
  logger::trace("ConnectionBuilder::registerTopicPublisher({}, {})", opInfo, topicInfo);
  auto op = store.get<OperationAPI>(Value::string(opInfo.at("fullName")));
  auto topicInfo2 = ObjectFactory::createTopic(store, topicInfo);
  auto connectionName = "topic_connection_" + Value::string(topicInfo.at("fullName")) + "_" + op->fullName();
  Value connectionInfo {
    {"name", connectionName},
    {"type", "event"},
    {"inlet", {
      {"name", "data"},
      {"broker", {
        {"typeName", "CoreBroker"}
      }},
      {"topic", 
        topicInfo
      }
    }},
    {"outlet", {
      {"operation", opInfo}
    }}
  };
  return op->outlet()->addConnection(ProxyBuilder::outgoingOperationConnectionProxy(connectionInfo, &store));
}
*/
/*
Value ConnectionBuilder::registerOperationProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker) {
    logger::trace("ConnectionBuilder::registerOperationProviderConnection({})", (ci));
    try {
        auto outputFullName = ci.at("output").at("info").at("fullName").stringValue();
        auto inputFullName = ci.at("input").at("info").at("fullName").stringValue();
        auto conName = ci.at("name").stringValue();
        auto argName = ci.at("input").at("target").at("name").stringValue();

        auto ret = ConnectionBuilder::_validateOutputConnectionInfo(store->getOperationOrTopic(outputFullName), ci);
        auto inputBroker = ObjectFactory::createBrokerProxy(*store, ci.at("input").at("broker"));
        auto ret2 = inputBroker->registerConsumerConnection(ret);
        if (ret2.isError()) {
          logger::error("ConnectionBduiler::registerOperationProviderConnection({}) failed. registerConsumerConnection failed.", ci);
          return ret2;
        }
        // リクエストが成功なら、こちらもConnectionを登録。
        auto ret3 = store->getOperationOrTopic(outputFullName)->addProviderConnection(providerConnection(ret2, inputBroker));
        if (ret3.isError()) {
            logger::error("ConnectionBuilder::registerOperationProviderConnection({}) failed. getOperationOrTopic({}) failed.", ci, outputFullName);
            // 登録が失敗ならConsumer側のConnectionを破棄。
            auto consumerConName = ret2.at("name").stringValue();
            inputBroker->removeConsumerConnection(inputFullName, argName, consumerConName);
            return Value::error(logger::error("ConnectionBuilder::registerOperationProviderConnection({}) for provider's broker failed. ", ci, ret2.getErrorMessage()));
        }// 登録成功ならciを返す
        logger::info("ConnectionBuilder::registerOperationProviderConnection({}) success.", ret3);
        return ret3;
    } catch (ValueTypeError& ex) {
        return Value::error(logger::error("ConnectionBuilder::registerOperationProviderConnection(" + str(ci) + ") failed. Exception: " + std::string(ex.what())));
    }
}
*/

/*
Value ConnectionBuilder::connect(const std::string& name, const ConnectionAPI::ConnectionType& type, const std::shared_ptr<OperationOutletAPI>& outlet, const std::shared_ptr<OperationInletAPI>& inlet) {
  logger::trace("Process::registerOperationConsumerConnection({}", (ci));
  inlet->addConnection(std::make_shared<Connection>(name, type, outlet, inlet);
  outlet->addConnection(std::make_shared<Connection>(name, type, outlet, inlet));            
}
*/

/**
 * もしConnectionInfoで指定されたBrokerが引数のbrokerでなければ，親プロセスに対して別のブローカーをリクエストする
 */
//static bool doProcessHasProviderOperation(ProcessStore* store, const std::string& fullName) {
//    return store->getOperation(fullName)->isNull();/
//}


//static std::shared_ptr<OperationBase> _getOperationOrTopic(ProcessStore* store, const Value& info) {
//  return store->getOperationOrTopic(info.at("fullName").stringValue());
//}

/*
Value ConnectionBuilder::_validateOutputConnectionInfo(std::shared_ptr<OperationBase> op, const Value& conInfo) {
  auto ret = conInfo;
  if (op->hasOutputConnectionRoute(ret)) {
    return Value::error(logger::warn("ConnectionBuilder::_validateOutputConnectionInfo failed. Provider already have the same connection route {}", str(ret.at("output"))));
  }
  int i = 2;
  while (op->hasOutputConnectionName(ret)) {
      logger::warn("In ConnectionBuilder::_validateOutputConnectionInfo, Provider already have the same connection name {}", str(ret.at("output")));
      ret["name"] = conInfo.at("name").stringValue() + "_" + std::to_string(i);
      ++i;
  }
  return ret;
}

Value ConnectionBuilder::_validateInputConnectionInfo(std::shared_ptr<OperationBase> op, const Value& conInfo) {
  auto ret = conInfo;
  if (op->hasInputConnectionRoute(ret)) {
    return Value::error(logger::warn("ConnectionBuilder::_validateInputConnectionInfo failed. Consumer already have the same connection route {}", str(ret.at("input"))));
  }
  int i = 2;
  while (op->hasInputConnectionName(ret)) {
      logger::warn("In ConnectionBuilder::_validateInputConnectionInfo, Consumer already have the same connection name {}", str(ret.at("input")));
      ret["name"] = conInfo.at("name").stringValue() + "_" + std::to_string(i);
      ++i;
  }
  return ret;
}

Value ConnectionBuilder::_validateInputConnectionInfo(std::shared_ptr<FSM> fsm, const Value& conInfo) {
  auto ret = conInfo;
  if (fsm->hasInputConnectionRoute(ret)) {
    return Value::error(logger::warn("ConnectionBuilder::_validateInputConnectionInfo failed. FSM already have the same connection route {}", str(ret.at("output"))));
  }
  int i = 2;
  while (fsm->hasInputConnectionName(ret)) {
      logger::warn("In ConnectionBuilder::_validateInputConnectionInfo, FSM already have the same connection name {}", str(ret.at("output")));
      ret["name"] = conInfo.at("name").stringValue() + "_" + std::to_string(i);
      ++i;
  }
  return ret;
}

Value ConnectionBuilder::registerConsumerConnection(ProcessStore* store, const Value& ci) {
  if (ci.isError()) return ci;

  auto type = getStringValue(ci.at("type"), "");
  if (type == "stateBind") {
    return ConnectionBuilder::registerFSMConsumerConnection(store, ci);
  }
  return ConnectionBuilder::registerOperationConsumerConnection(store, ci);
}

Value ConnectionBuilder::registerFSMConsumerConnection(ProcessStore* store, const Value& ci) {
  logger::trace("Process::registerFSMConsumerConnection({}", (ci));
  auto fullName = ci.at("input").at("info").at("fullName").stringValue();
  auto ret = ConnectionBuilder::_validateInputConnectionInfo(store->getFSM(fullName), ci);

  return store->getFSM(fullName)->addInputConnection(fsmConnection(ret,
            ObjectFactory::createBrokerProxy(*store, ci.at("input").at("broker"))));
}


Value ConnectionBuilder::registerOperationConsumerConnection(ProcessStore* store, const Value& ci) {
  logger::trace("Process::registerOperationConsumerConnection({}", (ci));
  auto fullName = ci.at("input").at("info").at("fullName").stringValue();
  auto ret = ConnectionBuilder::_validateInputConnectionInfo(store->getOperationOrTopic(fullName), ci);

  return store->getOperationOrTopic(fullName)->addConsumerConnection(consumerConnection(ret,
            ObjectFactory::createBrokerProxy(*store, ci.at("input").at("broker"))));
}
*/


Value ConnectionBuilder::deleteConnection(ProcessStore* store, const std::string& connectionFullName, BrokerAPI* receiverBroker /*=nullptr*/) {
  auto con = store->connection(connectionFullName);
  if (con->isNull()) {
    return Value::error(logger::error("ConnectionBuilder::deleteConnection() failed. Connection({}) not found.", connectionFullName));
  }

  auto info = con->info();

  
  //auto v_in = con->inlet()->removeConnection(connectionFullName);

  //auto v_out = con->outlet()->removeConnection(connectionFullName);
  

  //store->deleteConnection(connectionFullName);
    //auto outlet = ProxyBuilder::operationProxy(connectionInfo.at("outlet").at("operation"), store)->outlet();
    /*
  auto value = connectionInfo.at("inlet").at("operation");
  // TODO: brokerの項目がなかったらどうするか？
  auto inletBroker = store->brokerFactory(Value::string(value.at("broker").at("typeName")))->createProxy(value.at("broker"));
  inletBroker->operationInlet()->addConnection(Value::string(value.at("fullName")), Value::string(connectionInfo.at("inlet").at("name")), connectionInfo);
  // TODO: 名前が同じのがあったらどうするか？
  value = connectionInfo.at("outlet").at("operation");
  auto outletBroker = store->brokerFactory(Value::string(value.at("broker").at("typeName")))->createProxy(value.at("broker"));
  return outletBroker->operationOutlet()->addConnection(Value::string(value.at("fullName")), connectionInfo);
  */
  // return Value::error(logger::error("ConnectionBuilder::deleteConnection() failed."));

  return info;
}

/*
Value ConnectionBuilder::bindOperationToFSM(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker=nullptr) {
    logger::trace("ConnectionBuilder::bindOperationToFSM({})", (ci));
    try {
        auto outputFullName = ci.at("output").at("info").at("fullName").stringValue();
        auto inputFullName = ci.at("input").at("info").at("fullName").stringValue();
        auto conName = ci.at("name").stringValue();
        auto argName = ci.at("input").at("target").at("name").stringValue();

        auto ret = ConnectionBuilder::_validateOutputConnectionInfo(store->getOperationOrTopic(outputFullName), ci);
        auto inputBroker = ObjectFactory::createBrokerProxy(*store, ci.at("input").at("broker"));
        auto ret2 = inputBroker->registerConsumerConnection(ret);
        if (ret2.isError()) {
            logger::error("ConnectionBuilder::registerProviderConnection({}) failed. inputBroker->registerConsumerConnection({}) failed.", ci, ret); 
          return ret2;
        }
        // リクエストが成功なら、こちらもConnectionを登録。
        auto ret3 = store->getOperationOrTopic(outputFullName)->addProviderConnection(fsmConnection(ret2, inputBroker));
        if (ret3.isError()) {
            // 登録が失敗ならConsumer側のConnectionを破棄。
            auto consumerConName = ret2.at("name").stringValue();
            inputBroker->removeConsumerConnection(inputFullName, argName, consumerConName);
            return Value::error(logger::error("ConnectionBuilder::registerProviderConnection({}) for provider's broker failed. ", ci, ret2.getErrorMessage()));
        }// 登録成功ならciを返す
        return ret3;
    } catch (ValueTypeError& ex) {
        return Value::error(logger::error("ConnectionBuilder::registerProviderConnection(" + str(ci) + ") failed. Exception: " + std::string(ex.what())));
    }
}

Value ConnectionBuilder::unbindOperationToFSM(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker *=nullptr*) {

}

Value ConnectionBuilder::registerProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker/*=nullptr*) {
  logger::trace("ConnectionBuilder::registerOperationProviderConnection({})", ci);
  return ConnectionBuilder::registerOperationProviderConnection(store, ci, receiverBroker);
}


Value ConnectionBuilder::deleteConsumerConnection(ProcessStore* store, const std::string& fullName, const std::string& targetArgName, const std::string& conName) {
  return ConnectionBuilder::deleteOperationConsumerConnection(store, fullName, targetArgName, conName);
}
  
Value ConnectionBuilder::deleteOperationConsumerConnection(ProcessStore* store, const std::string& fullName, const std::string& targetArgName, const std::string& conName) {
  logger::trace("Process::deleteOperationConsumerConnection({}, {}, {}", fullName, targetArgName, conName);
  return store->getOperation(fullName)->removeConsumerConnection(targetArgName, conName);
}

Value ConnectionBuilder::deleteProviderConnection(ProcessStore* store, const std::string& fullName, const std::string& conName) {
  logger::trace("Process::deleteOperationConsumerConnection({}, {}", fullName, conName);
  return ConnectionBuilder::deleteOperationProviderConnection(store, fullName, conName);
}


Value ConnectionBuilder::deleteOperationProviderConnection(ProcessStore* store, const std::string& fullName, const std::string& conName) {
    logger::trace("Process::deleteOperationProviderConnection({}, {})", fullName, conName);
    auto provider = store->getOperation(fullName);
    const auto& connection = provider->getOutputConnection(conName);
    if (connection.isNull()) {
      return Value::error(logger::error("ConnectionBuilder::deleteProviderConnection failed. ProviderOperation can not find connection named(" + conName + ")"));
    }
    auto ret = connection.info();
    auto consumerName = connection.info().at("input").at("info").at("fullName").stringValue();
    auto targetArgName = connection.info().at("input").at("target").at("name").stringValue();

    //auto consumerBroker = createBrokerProxy(ret.at("input").at("broker"));
    auto consumerBroker = ObjectFactory::createBrokerProxy(*store, ret.at("input").at("broker"));
    if (!consumerBroker) {
      return Value::error(logger::error("Process::deleteProviderConnection failed. ConsumerBrokerProxy cannot be created."));
    }
    ret = consumerBroker->removeConsumerConnection(consumerName, targetArgName, conName);
    if (ret.isError()) {
      return Value::error(logger::error("Process::deleteProviderConnection failed. RemoveConsumerConnection failed."));
    }
    return provider->removeProviderConnection(ret);
}
*/

/*
Value ConnectionBuilder::registerTopicSubscriber(ProcessStore* store, const Value& opInfo, const std::string& argName, const Value& topicInfo) {
  logger::trace("ConnectionBuilder::registerTopicSubscriber({}, {}, {})", opInfo, argName, topicInfo);
  if (!topicInfo.isError() && !opInfo.isError()) {
    auto fullName = opInfo.at("fullName").stringValue();
    // if (opInfo.hasKey("ownerContainerInstanceName")) {
    //   instanceName = opInfo.at("ownerContainerInstanceName").stringValue() + ":" + instanceName;
    // }
    return registerProviderConnection(store, {
      {
        {"name", "topic_connection01"},
        {"type", "event"},
        {"input", {
          {"info", { 
            {"fullName", fullName}
          }},
          {"broker", {
            {"typeName", "CoreBroker"}
          }},
          {"target", {{"name", argName}}}
        }},
        {"output", {
          {"info", {
            {"fullName", topicInfo.at("fullName")},
            {"topicType", "BasicTopic"},
            {"broker", {
              {"typeName", "CoreBroker"}
            }}
          }}
        }}
      }
    });
  }
  return Value::error(logger::error("Register Topic Subscriber failed: {}", topicInfo));
}
*/
