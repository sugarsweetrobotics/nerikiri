
#include "process_builder.h"
#include "moduleloader.h"
#include "../objectfactory.h"
#include "../connection/connection_builder.h"
#include "../topic/topic_factory.h"

#include "../pose/static_transformation_operation.h"

using namespace juiz;
/**
 * OperationFactoryの読み込み
 */
void ProcessBuilder::preloadOperations(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::preloadOperations() entry");
  config.at("operations").at("preload").const_list_for_each([&store, &config, &path](auto value) {
    ModuleLoader::loadOperationFactory(store, {"./", path}, {
      {"typeName", value}, {"load_paths", config.at("operations").at("load_paths")}
    });
  });

  config.at("operations").at("precreate").const_list_for_each([&store](auto& oinfo) {
    auto opInfo = ObjectFactory::createOperation(store, oinfo);
    auto op = store.get<OperationAPI>(Value::string(opInfo.at("fullName")));
  });

  /// ここでコンテナ間の位置オフセット用Operationを登録
  store.add<OperationFactoryAPI>(std::shared_ptr<OperationFactoryAPI>( static_cast<OperationFactoryAPI*>(static_transformation_operation()) ));

  logger::trace("ProcessBuilder::preloadOperations() exit");
}


void ProcessBuilder::preloadContainers(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::preloadContainers(path={}) entry", path);
  /// オブジェクト型のリストでの定義だった場合の読み込みルール
  config.at("containers").at("preload").const_list_for_each([&store, &config, &path](auto value) {
    ModuleLoader::loadContainerFactory(store, {"./", path}, {
      {"typeName", value.at("typeName")}, {"load_paths", config.at("containers").at("load_paths")}
    });
    if (value.hasKey("operations")) {
      value.at("operations").const_list_for_each([&config, &store, &value, &path](auto& v) {
        ModuleLoader::loadContainerOperationFactory(store, {"./", path}, {
          {"typeName", v}, {"container_name", value.at("typeName")}, {"load_paths", config["containers"]["load_paths"] }
        });
      });
    }
  });
  /// よりシンプルな，単一オブジェクト内でのキー＆バリュー型の定義
  /// キー：コンテナ名
  /// バリュー：　リスト．Operationの名前のリスト
  config.at("containers").at("preload").const_object_for_each([&store, &config, &path](auto key, auto value) {
    ModuleLoader::loadContainerFactory(store, {"./", path}, {
      {"typeName", key}, {"load_paths", config.at("containers").at("load_paths")}
    });
    value.const_list_for_each([&config, &store, &key, &path](auto& v) {
      ModuleLoader::loadContainerOperationFactory(store, {"./", path}, {
        {"typeName", v}, {"container_name", key}, {"load_paths", config["containers"]["load_paths"] }
      });
    });
  });
  
  config.at("containers").at("precreate").const_list_for_each([&store](auto info) {
    /// ここでコンテナを作成する
    auto cInfo = ObjectFactory::createContainer(store, info);
    if (info.hasKey("operations")) {
      info.at("operations").const_list_for_each([&store, &cInfo](auto value) {
        auto opInfo = ObjectFactory::createContainerOperation(store, cInfo, value);
        auto c = store.get<ContainerAPI>(Value::string(cInfo.at("fullName")));
        auto cop = c->operation(Value::string(opInfo.at("fullName")));
      });
    }
  });

  config.at("containers").at("transformation").const_list_for_each([&store](auto tfInfo) {
    if (tfInfo.hasKey("typeName") && Value::string(tfInfo["typeName"]) == "static") {
      auto offset = toPose3D(tfInfo["offset"]);
      auto from_c = store.get<ContainerAPI>(Value::string(tfInfo["from"]["fullName"]));
      auto to_c = store.get<ContainerAPI>(Value::string(tfInfo["to"]["fullName"]));
      auto fullName = "offset_" + from_c->fullName() + "_" + to_c->fullName();
      auto tfOp = std::dynamic_pointer_cast<OperationAPI>(store.get<OperationFactoryAPI> ("static_transformation_operation")->create(fullName, {
        {"defaultArgs", {
          {"offset", tfInfo["offset"]}
        }}
      }));
      store.add<OperationAPI>(tfOp);

      const std::string connectionNameFrom = "tfcon_" + from_c->fullName() + "_" + fullName;
      auto fromOp = store.get<OperationAPI>(from_c->fullName() + ":" + "container_set_pose.ope");
      const std::string connectionNameTo = "tfcon_" + fullName + "_" + to_c->fullName();
      auto toOp = store.get<OperationAPI>(to_c->fullName() + ":" + "container_set_pose.ope");

      /// TODO: 入力の名前とタイプを確認しないといけないよ
      auto result1 = juiz::connect(coreBroker(store), connectionNameFrom, tfOp->inlet("pose"), fromOp->outlet());
      auto result2 = juiz::connect(coreBroker(store), connectionNameTo, toOp->inlet("pose"), tfOp->outlet());
    } else if (tfInfo.hasKey("fullName")) {
      auto fullName = Value::string(tfInfo["fullName"]);
      auto tfOp = store.get<OperationAPI>(fullName);
      auto from_c = store.get<ContainerAPI>(Value::string(tfInfo["from"]["fullName"]));
      auto to_c = store.get<ContainerAPI>(Value::string(tfInfo["to"]["fullName"]));

      const std::string connectionNameFrom = "tfcon_" + from_c->fullName() + "_" + fullName;
      auto fromOp = store.get<OperationAPI>(from_c->fullName() + ":" + "container_set_pose.ope");
      const std::string connectionNameTo = "tfcon_" + fullName + "_" + to_c->fullName();
      auto toOp = store.get<OperationAPI>(to_c->fullName() + ":" + "container_set_pose.ope");

      /// TODO: 入力の名前とタイプを確認しないといけないよ
      auto result1 = juiz::connect(coreBroker(store), connectionNameFrom, tfOp->inlet("pose"), fromOp->outlet());
      auto result2 = juiz::connect(coreBroker(store), connectionNameTo, toOp->inlet("pose"), tfOp->outlet());
    } else {
      logger::error("ProcessBuilder::preloadContainers() failed. Invalid transformation typename (tfInfo={})", tfInfo);
    }
  });

  logger::trace("ProcessBuilder::preloadContainers() exit");
}

void ProcessBuilder::preloadFSMs(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::_preloadFSMs() entry");
  config.at("fsms").at("precreate").const_list_for_each([&store](auto& value) {
    ObjectFactory::createFSM(store, value);
  });
  logger::trace("Process::_preloadFSMs() exit");
}


void ProcessBuilder::preloadExecutionContexts(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::_preloadExecutionContexts() entry");
  config.at("ecs").at("preload").const_list_for_each([&store, &config, &path](auto& value) {
    ModuleLoader::loadExecutionContextFactory(store, {"./", path}, {
      {"typeName", value}, {"load_paths", config.at("ecs").at("load_paths")}
    });
  });
  config.at("ecs").at("precreate").const_list_for_each([&store](auto& value) {
    ObjectFactory::createExecutionContext(store, value);
  });

  config.at("ecs").at("bind").const_list_for_each([&store](auto value) {
    auto ecName = getStringValue(value["fullName"], "");
    auto ec = store.get<ContainerAPI>(ecName);
    if (ec->isNull()) {
      logger::error("ProcessBuilder::preloadExecutionContext failed. Loading Precreated EC named '{}' failed. Not found.", ecName);
      return;
    }
    auto activate_started_ope = ec->operation("activate_state_started.ope");
    if (activate_started_ope->isNull()) {
      logger::error("ProcessBuilder::preloadExecutionContext failed. Loading Precreated EC's activation function named '{}' failed. Not found.", ecName + ":activate_state_started.ope");
      return;
    }
    value["operations"].const_list_for_each([&store, &ec, &activate_started_ope](auto& v) {
      auto opProxy =  store.operationProxy(v);
      juiz::connect(coreBroker(store), ec->fullName() + "_bind_" + Value::string(v["fullName"]), 
        opProxy->inlet("__event__"), activate_started_ope->outlet(), {});
      //ec->bind(store.operation(Value::string(v.at("fullName"))));
    });
  });
  logger::trace("ProcessBuilder::_preloadExecutionContexts() exit");
}


void ProcessBuilder::preloadAnchors(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::_preloadAnchors() entry");
  config.at("anchors").const_list_for_each([&store, &config](auto& value) {
    auto returnValue = ObjectFactory::createAnchor(store, value);
    auto anchor = store.get<ContainerAPI>(Value::string(value["fullName"]));

    auto tgt = value.at("target");
    auto tgtCtn = store.get<ContainerAPI>(Value::string(tgt["fullName"]));
    if (tgtCtn->isNull()) {
      logger::error("ProcessBuilder::preloadAnchor failed. Loading Precreated COntainer named '{}' failed. Not found.", tgt["fullName"]);
      return;
    }
    auto set_basepose_ope = tgtCtn->setPoseOperation();
    auto activate_started_ope = anchor->operation("activate_state_started.ope");
    if (activate_started_ope->isNull()) {
      logger::error("ProcessBuilder::preloadExecutionContext failed. Loading Precreated EC's activation function named '{}' failed. Not found.", Value::string(tgt["fullName"]) + ":activate_state_started.ope");
      return;
    }
    juiz::connect(coreBroker(store), tgtCtn->fullName() + "_bind_" + anchor->fullName(), 
        set_basepose_ope->inlet("pose"), activate_started_ope->outlet(), {});
    if (Value::boolValue(value["autoStart"], false)) {
      anchor->operation("activate_state_started.ope")->execute();
    }
  });
  logger::trace("ProcessBuilder::_preloadAnchors() exit");
}

/**
 * 
 */
void ProcessBuilder::preStartFSMs(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::_preStartFSMs() entry");
  config.at("fsms").at("bind").const_list_for_each([&store](const auto& bindInfo) {
     {
      auto fullName = Value::string(bindInfo["fullName"]);
      auto state    = Value::string(bindInfo["state"]);
      auto fsm = store.get<ContainerAPI>(fullName);
      auto stateOp = store.get<OperationAPI>(fullName + ":activate_state_" + state + ".ope");
      bindInfo["operations"].const_list_for_each([&store, &stateOp](auto& opInfo) {
        auto name = stateOp->fullName() + "_state_connection_" + Value::string(opInfo["fullName"]);
        Value option = {};
        if (opInfo.hasKey("argument")) {
          option["argument"] = opInfo["argument"];
        }
        juiz::connect(coreBroker(store), name, store.operationProxy(opInfo)->inlet("__argument__"), stateOp->outlet(), option);
      });
      bindInfo["ecs"].const_list_for_each([&store, &stateOp](auto& ecInfo) {
        auto name = stateOp->fullName() + "_state_connection_" + Value::string(ecInfo["fullName"]) + "_" + Value::string(ecInfo["state"]) ;
        Value ecStateOpInfo {
          {"fullName", Value::string(ecInfo["fullName"]) + ":activate_state_" + Value::string(ecInfo["state"]) + ".ope"}
        };
        if (ecInfo.hasKey("broker")) {
          ecStateOpInfo["broker"] = ecInfo["broker"];
        }
        juiz::connect(coreBroker(store), name, store.operationProxy(ecStateOpInfo)->inlet("__event__"), stateOp->outlet());
      });
     }
  });
    /*
    auto fsmInfo = bindInfo.at("fsm");
    auto opInfo = bindInfo.at("operation");
    store.fsm(Value::string(fsmInfo.at("fullName")))->fsmState(Value::string(fsmInfo.at("state")))->bind(
      store.operation(Value::string(opInfo.at("fullName"))),
      opInfo.at("argument")
    );
    */
    /*
    if (opInfo.hasKey("argument")) {
      this->store()->getFSM(fsmInfo)->bindStateToOperation(fsmInfo.at("state").stringValue(), 
        this->store()->getAllOperation(opInfo), opInfo.at("argument")
      );
    } else {
      this->store()->getFSM(fsmInfo)->bindStateToOperation(fsmInfo.at("state").stringValue(), 
        this->store()->getAllOperation(opInfo)
      );
    }
    *
  }); 
    /* 
    config.at("fsms").at("bind").at("operations").const_list_for_each([&store](auto& bindInfo) {
      auto fsmInfo = bindInfo.at("fsm");
      auto opInfo = bindInfo.at("operation");
      store.fsm(Value::string(fsmInfo.at("fullName")))->fsmState(Value::string(fsmInfo.at("state")))->bind(
        store.operation(Value::string(opInfo.at("fullName"))),
        opInfo.at("argument")
      );
      /*
      if (opInfo.hasKey("argument")) {
        this->store()->getFSM(fsmInfo)->bindStateToOperation(fsmInfo.at("state").stringValue(), 
          this->store()->getAllOperation(opInfo), opInfo.at("argument")
        );
      } else {
        this->store()->getFSM(fsmInfo)->bindStateToOperation(fsmInfo.at("state").stringValue(), 
          this->store()->getAllOperation(opInfo)
        );
      }
      *
    }); */
    /* 
    auto c = config.at("fsms").at("bind");
    auto ecs = c.at("ecs");
    */
    /*
    ecs.list_for_each([this](auto& value) {
      // TODO: ECへのバインド
      //store()->fsm(Value::string(value.at("fsm").at("fullName")))
      
      auto fsmInfo = value.at("fsm");
      auto ecInfo = value.at("ec");
      if (ecInfo.at("state").stringValue() == "started") {
        this->store()->getFSM(fsmInfo)->bindStateToECStart(fsmInfo.at("state").stringValue(), 
          this->store()->getExecutionContext(ecInfo)
        );
      } else if (ecInfo.at("state").stringValue() == "stopped") {
        this->store()->getFSM(fsmInfo)->bindStateToECStop(fsmInfo.at("state").stringValue(), 
          this->store()->getExecutionContext(ecInfo)
        );
      }
      
    });
    */
  //} catch (juiz::ValueTypeError& e) {
  //  logger::debug("Process::_preloadFSMs(). ValueTypeException:{}", e.what());
  //}

  logger::trace("ProcessBuilder::_preStartFSMs() exit");
}

void ProcessBuilder::preStartExecutionContexts(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::preStartExecutionContexts() entry");
  juiz::getListValue(config, "ecs.start").const_list_for_each([&store](const auto& value) {
   // store.executionContext(value.stringValue())->start();
   // TODO: ここでECをスタートする
  });
    /*
    auto c = config_.at("ecs").at("start");
    c.list_for_each([this](auto& value) {
      this->store()->getExecutionContext(value.stringValue())->start();
    });
    */
  logger::trace("ProcessBuilder::preStartExecutionContexts() exit");
}

void ProcessBuilder::preloadBrokers(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::preloadBrokers(info={}) entry", config["brokers"]);
  config.at("brokers").at("preload").const_list_for_each([&store, &config, &path](auto& value) {
    ModuleLoader::loadBrokerFactory(store, {"./", path}, {
      {"typeName", value}, {"load_paths", config["brokers"]["load_paths"]}
    });
  });
  config.at("brokers").at("precreate").const_list_for_each([&store](auto& value) {
    ObjectFactory::createBroker(store, value);
  });

  logger::trace("ProcessBuilder::preloadBrokers() exit");
}

void ProcessBuilder::preloadConnections(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::_preloadConnections() entry");
  config.at("connections").const_list_for_each([&store](auto& value) {
   // ConnectionBuilder::registerProviderConnection(store(), value);
   /// TODO: ここではStateBindも来るかもしれない
    ConnectionBuilder::createOperationConnection(store, value);
  });
  logger::trace("ProcessBuilder::_preloadConnections() exit");
}

Value ProcessBuilder::publishTopic(ProcessStore& store, const std::shared_ptr<ClientProxyAPI>& broker, const Value& opInfo, const Value& topicInfo) {
  logger::trace("ProcessBuilder::publishTopic({}, {})", opInfo, topicInfo);
  auto op = store.get<OperationAPI>(Value::string(opInfo.at("fullName")));
  auto topicInfo2 = ObjectFactory::createTopic(store, topicInfo);
  auto topic = store.get<TopicAPI>(Value::string(topicInfo.at("fullName")));
  auto connectionName = "topic_connection_" + Value::string(topicInfo.at("fullName")) + "_" + op->fullName();

  return juiz::connect(broker, connectionName, topic->inlet("data"), op->outlet());
}


Value ProcessBuilder::subscribeTopic(ProcessStore& store, const std::shared_ptr<ClientProxyAPI>& broker, const Value& opInfo, const std::string& argName, const Value& topicInfo) {
  logger::trace("ProcessBuilder::subscribeTopic({}, {}, {}) called", opInfo, argName, topicInfo);
  
  auto op = store.get<OperationAPI>(Value::string(opInfo.at("fullName")));
  auto topicInfo2 = ObjectFactory::createTopic(store, topicInfo);
  auto topic = store.get<TopicAPI>(Value::string(topicInfo.at("fullName")));
  auto connectionName = "topic_connection_" + op->fullName() + "_" + argName + "_" + Value::string(topicInfo.at("fullName"));

  return juiz::connect(broker, connectionName, op->inlet(argName), topic->outlet());
}

// ここでOperationの情報からTopicを作成する
void _parseOperationInfo(ProcessStore& store, const std::shared_ptr<ClientProxyAPI>& broker, const Value& opInfo) {
  logger::trace("{} _parseOperationInfo(info={}) called.", __FILE__, opInfo);
  if (opInfo["publish"].isStringValue()) {
    ProcessBuilder::publishTopic(store, broker, opInfo, {{"fullName", opInfo["publish"]}});
  } else {
    opInfo["publish"].const_list_for_each([&opInfo, &store, &broker](auto &topicInfo) {
      if (topicInfo.isStringValue()) {
        ProcessBuilder::publishTopic(store, broker, opInfo, {{"fullName", topicInfo}});
      } else {
        ProcessBuilder::publishTopic(store, broker, opInfo, topicInfo);
      }
    });
  } 
  opInfo["subscribe"].const_object_for_each([&opInfo, &store, &broker](auto& argName, auto &argInfo) {
    if (argInfo.isStringValue()) {
      ProcessBuilder::subscribeTopic(store, broker, opInfo, argName, {{"fullName", argInfo}});
    } else {
      argInfo.const_list_for_each([&opInfo, &store, &broker, &argName](auto &topicInfo) {
        if (topicInfo.isStringValue()) {
          ProcessBuilder::subscribeTopic(store, broker, opInfo, argName, {{"fullName", topicInfo}});
        } else {
          ProcessBuilder::subscribeTopic(store, broker, opInfo, argName, topicInfo);
        }
      });
    }
  });

}

void ProcessBuilder::preloadTopics(ProcessStore& store, const std::shared_ptr<ClientProxyAPI>& broker, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::preloadTopics() entry");
  try {
    std::vector<Value> topicInfos;
    config["operations"]["precreate"].const_list_for_each([&store, &broker](auto &opInfo) {
      _parseOperationInfo(store, broker, opInfo);
    });

    config.at("containers").at("precreate").const_list_for_each([&store, &broker](auto &conInfo) {
      conInfo["operations"].const_list_for_each([&conInfo, &store, &broker](auto &opInfo_) {
        Value opInfo = opInfo_;
        auto fullNameValue = conInfo["fullName"];
        if (!conInfo["fullName"].isStringValue()) {
          fullNameValue = conInfo["instanceName"];
        }
        opInfo["fullName"] = fullNameValue.stringValue() + ":" + opInfo_["instanceName"].stringValue();
        _parseOperationInfo(store, broker, opInfo);
      });
    });
  } catch (juiz::ValueTypeError& e) {
    logger::debug("ProcessBuilder::preloadTopics(). ValueTypeException:{}", e.what());
  } 
  logger::trace("ProcessBuilder::preloadTopics() exit");

}

void ProcessBuilder::preloadCallbacksOnStarted(ProcessStore& store, const Value& config, const std::string& path) {
  try {
    auto c = config.at("callbacks");
    c.const_list_for_each([store](auto& value) {
      // TODO: コールバックね
      if (value.at("name").stringValue() == "on_started") {
        value.at("target").const_list_for_each([&store](auto& v) {
          auto opName = v.at("fullName").stringValue();
          auto argument = v.at("argument");
          store.get<OperationAPI>(opName)->call(argument);
        });
      }
    });
  } catch (ValueTypeError& ex) {
    logger::error("Process::_preloadCallbacksOnStarted failed. Exception: {}", ex.what());
  }
}