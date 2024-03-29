
#include "process_builder.h"
#include "moduleloader.h"
#include "../objectfactory.h"
#include "../connection/connection_builder.h"
#include "../topic/topic_factory.h"

#include "../pose/static_transformation_operation.h"
#include "../pose/dynamic_transformation_operation.h"
#include <juiz/path_parser.h>

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
  store.add<OperationFactoryAPI>(std::shared_ptr<OperationFactoryAPI>( static_cast<OperationFactoryAPI*>(dynamic_transformation_operation()) ));

  logger::trace("ProcessBuilder::preloadOperations() exit");
}

namespace {
  void loadContainerFactory(ProcessStore& store, const std::string& typeName, const std::vector<std::string>& load_paths) {
    const auto dllproxy = ModuleLoader::loadDLL(typeName, load_paths);
    if (dllproxy) {
      store.addDLLProxy(dllproxy);
      store.add(ModuleLoader::loadContainerFactory(dllproxy, typeName)); /// これでコンテナのファクトリを作成
    }
  }

  void loadContainerOperationFactory(ProcessStore& store, const std::string& opTypeName, const std::vector<std::string>& load_paths) {
    const auto opDLLProxy = ModuleLoader::loadDLL(opTypeName, load_paths);
    if (opDLLProxy) {
      store.addDLLProxy(opDLLProxy);
      store.add(ModuleLoader::loadContainerOperationFactory(opDLLProxy, opTypeName));

    }
  }
}

void ProcessBuilder::preloadContainerFactories(ProcessStore& store, const Value& containerConfig, const std::string& path) {
  logger::trace("ProcessBuilder::preloadContainers(containerConfig={} path={}) entry", containerConfig, path);
  std::vector<std::string> load_paths = {"./", path};
  containerConfig["load_paths"].const_list_for_each([&load_paths](auto value) {
    load_paths.push_back(getStringValue(value, ""));
  });
  /// オブジェクト型のリストでの定義だった場合の読み込みルール
  containerConfig["preload"].const_list_for_each([&store, &load_paths](auto value) {
    /// ここでコンテナごとにDLLをロードする．
    const std::string typeName = getStringValue(value["typeName"], "");
    loadContainerFactory(store, typeName, load_paths);
    value["operations"].const_list_for_each([&store, &typeName, &load_paths](auto opValue) {
      /// コンテナの記述内にoperationsがあれば，それは文字列のリストなので，ContainerOperationとして読み込む．
      loadContainerOperationFactory(store, typeName + "_" + opValue.stringValue(), load_paths);
    });
  });

  /// キーバリュー型のディクショナリでの定義の場合の読み込みルール
  containerConfig["preload"].const_object_for_each([&store, &load_paths](auto containerName, auto value) {
    const std::string typeName = getStringValue(containerName, "");
    loadContainerFactory(store, typeName, load_paths);
    /// もしコンテナ名
    value.const_list_for_each([&store, &typeName, &load_paths](auto& opValue) {
      loadContainerOperationFactory(store, typeName + "_" + opValue.stringValue(), load_paths);
    });
  
    if (value.hasKey("mesh")) {
      store.get<ContainerFactoryAPI>(containerName)->setMeshData(ContainerFactoryAPI::loadMesh(value["mesh"]));
    }
    value["operations"].const_list_for_each([&store, &typeName, &load_paths](auto& opValue) {
      loadContainerOperationFactory(store, typeName + "_" + opValue.stringValue(), load_paths);
    });
  });
}


void ProcessBuilder::precreateContainers(ProcessStore& store, const Value& containerConfig) {
  containerConfig["precreate"].const_list_for_each([&store](auto info) {
    /// ここでコンテナを作成する
    const auto cInfo = ObjectFactory::createContainer(store, info);
    info["operations"].const_list_for_each([&store, &cInfo](auto value) {
      auto opInfo = ObjectFactory::createContainerOperation(store, cInfo, value);
    });
    if (info.hasKey("mesh")) {
      if (info["mesh"].hasKey("include")) {
        store.get<ContainerAPI>(cInfo["fullName"])->setMeshData(ContainerFactoryAPI::loadMesh(info["mesh"]));
      } else {
        store.get<ContainerAPI>(cInfo["fullName"])->setMeshData(info["mesh"]);
      }
    }
  });
}

namespace {
  void setupStaticTransformation(ProcessStore& store, const Value& tfInfo) {
    //auto offset = toPose3D(tfInfo["offset"]);

    const auto from_c = store.get<ContainerAPI>(tfInfo["from"]["fullName"]);
    const auto to_c = store.get<ContainerAPI>(tfInfo["to"]["fullName"]);
    const auto fullName = "offset_" + from_c->fullName() + "_" + to_c->fullName();
    auto tfOp = std::dynamic_pointer_cast<OperationAPI>(store.get<OperationFactoryAPI> ("static_transformation_operation")->create(fullName, {
      {"defaultArgs", {
        {"offset", tfInfo["offset"]}
      }}
    }));
    store.add<OperationAPI>(tfOp);

    const auto fromOp = store.get<OperationAPI>(from_c->fullName() + ":" + "container_set_pose.ope");
    const auto toOp = store.get<OperationAPI>(to_c->fullName() + ":" + "container_set_pose.ope");

    /// TODO: 入力の名前とタイプを確認しないといけないよ
    auto result1 = juiz::connect(coreBroker(store), "tfcon_" + from_c->fullName() + "_" + fullName, tfOp->inlet("pose"), fromOp->outlet());
    auto result2 = juiz::connect(coreBroker(store), "tfcon_" + fullName + "_" + to_c->fullName(), toOp->inlet("pose"), tfOp->outlet());
  }

  void setupDynamicTransformation(ProcessStore& store, const Value& tfInfo) {
    const auto from_c = store.get<ContainerAPI>(Value::string(tfInfo["from"]["fullName"]));
    const auto to_c = store.get<ContainerAPI>(Value::string(tfInfo["to"]["fullName"]));
    const auto fullName = "input_" + from_c->fullName() + "_" + to_c->fullName();
    const auto tfOp = std::dynamic_pointer_cast<OperationAPI>(store.get<OperationFactoryAPI> ("dynamic_transformation_operation")->create(fullName, {
      }
    ));
    store.add<OperationAPI>(tfOp);

    auto fromOp = store.get<OperationAPI>(from_c->fullName() + ":" + "container_set_pose.ope");
    auto toOp = store.get<OperationAPI>(to_c->fullName() + ":" + "container_set_pose.ope");
    auto dynOp = store.get<OperationAPI>(tfInfo["input"]["fullName"]);

    /// TODO: 入力の名前とタイプを確認しないといけないよ
    auto result1 = juiz::connect(coreBroker(store), "dynamic_tfcon_" + from_c->fullName() + "_" + fullName, tfOp->inlet("pose"), fromOp->outlet());
    auto result2 = juiz::connect(coreBroker(store), "dynamic_tfcon_" + fullName + "_" + to_c->fullName(), toOp->inlet("pose"), tfOp->outlet());
    auto result3 = juiz::connect(coreBroker(store), "dynamic_dyncon_" + tfInfo["input"]["fullName"].stringValue() + "_" + fullName, tfOp->inlet("input"), dynOp->outlet());
  }


  void setupNamedTransformation(ProcessStore& store, const Value& tfInfo) {
    const auto fullName = Value::string(tfInfo["fullName"]);
    const auto tfOp = store.get<OperationAPI>(fullName);
    const auto from_c = store.get<ContainerAPI>(tfInfo["from"]["fullName"]);
    const auto to_c = store.get<ContainerAPI>(tfInfo["to"]["fullName"]);

    const auto fromOp = store.get<OperationAPI>(from_c->fullName() + ":" + "container_set_pose.ope"); /// 当該変換が受け取る出力
    const auto toOp = store.get<OperationAPI>(to_c->fullName() + ":" + "container_set_pose.ope"); /// 当該変換が出力すべき相手入力

    /// TODO: 入力の名前とタイプを確認しないといけないよ
    auto result1 = juiz::connect(coreBroker(store), "tfcon_" + from_c->fullName() + "_" + fullName, tfOp->inlet("pose"), fromOp->outlet());
    auto result2 = juiz::connect(coreBroker(store), "tfcon_" + fullName + "_" + to_c->fullName(), toOp->inlet("pose"), tfOp->outlet());
  }

}

void ProcessBuilder::presetContainerTransformation(ProcessStore& store, const Value& containerConfig) {
  containerConfig["transformation"].const_list_for_each([&store](auto tfInfo) {
    if (tfInfo.hasKey("typeName") && Value::string(tfInfo["typeName"]) == "static") { /// スタティックな変換
      setupStaticTransformation(store, tfInfo);
    } else if (tfInfo.hasKey("typeName") && Value::string(tfInfo["typeName"]) == "dynamic") { /// ダイナミックな変換
      setupDynamicTransformation(store, tfInfo);
    } else if (tfInfo.hasKey("fullName")) { // 名前のある変換
      setupNamedTransformation(store, tfInfo);
    } else {
      logger::error("ProcessBuilder::preloadContainers() failed. Invalid transformation typename (tfInfo={})", tfInfo);
    }
  });
  logger::trace("ProcessBuilder::preloadContainers() exit");
}


void ProcessBuilder::preloadContainers(ProcessStore& store, const Value& config, const std::string& path) {
  preloadContainerFactories(store, config["containers"], path);
  precreateContainers(store, config["containers"]);
  presetContainerTransformation(store, config["containers"]);
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
  config["ecs"]["preload"].const_list_for_each([&store, &config, &path](auto& value) {
    ModuleLoader::loadExecutionContextFactory(store, {"./", path}, {
      {"typeName", value}, {"load_paths", config["ecs"]["load_paths"]}
    });
  });
  config["ecs"]["precreate"].const_list_for_each([&store](auto& value) {
    auto ecInfo = ObjectFactory::createExecutionContext(store, value);
    auto ec = store.get<ContainerAPI>(ecInfo["fullName"]);
    value["bind"].const_list_for_each([&store, &ecInfo](auto& opInfo) {
      juiz::bind(store, ecInfo, opInfo);
    });
  });

  config["ecs"]["bind"].const_list_for_each([&store](auto value) {
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

    if (getStringValue(value["typeName"], "") == "DynamicPeriodicAnchor") {
      auto input_ope = store.get<OperationAPI>(value["input"]["fullName"].stringValue());
      if (input_ope->isNull()) {
        logger::error("ProcessBuilder::preloadExecutionContext failed. Loading input operation named '{}' failed. Not found.", Value::string(tgt["fullName"]) + ":activate_state_started.ope");
        return;
      }
      if (value["input"].hasKey("connectionType") && getStringValue(value["input"]["connectionType"], "") == "pull") {
        juiz::connect(coreBroker(store), input_ope->fullName() + "_bind_" + anchor->fullName(), 
          activate_started_ope->inlet("input"), input_ope->outlet(), {{"type", "pull"}});
      } else {
        juiz::connect(coreBroker(store), input_ope->fullName() + "_bind_" + anchor->fullName(), 
          activate_started_ope->inlet("input"), input_ope->outlet(), {});
      }
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
  logger::trace("ProcessBuilder::preStartFSMs() entry");
  config["fsms"]["precreate"].const_list_for_each([&store](auto& fsmInfo) {
    const auto fsm = store.get<ContainerAPI>(fsmInfo["fullName"]);
    fsmInfo["states"].const_list_for_each([&fsm, &store](auto& stateInfo) {
      const auto stateOp = store.get<OperationAPI>(fsm->fullName() + ":activate_state_" + stateInfo["name"].stringValue() + ".ope");
      stateInfo["bind"].const_list_for_each([&fsm, &stateOp, &store](auto& opInfo) {
        std::string opFullName;
        if (opInfo.isStringValue()) {
          opFullName = PathParser::operationFullName(opInfo.stringValue());
        } else {
          opFullName = getStringValue(opInfo["fullName"], "");
        }
        const auto name = stateOp->fullName() + "_state_connection_" + opFullName;
        logger::trace(" - FSM connection: {}", name);
        Value option = {};
        if (opInfo.hasKey("argument")) {
          option["argument"] = opInfo["argument"];
        }
        const auto opProxy = store.operationProxy(opInfo);
        if (!opProxy->isNull()) {
          auto result = juiz::connect(coreBroker(store), name, opProxy->inlet("__argument__"), stateOp->outlet(), option);
          if (result.isError()) {
            logger::error("ProcessBuilder::preStartFSMs() failed. juiz::connect returns error. {}", result);
          }
          return;
        } 

        std::string ecFullName, ecStateName;
        if (opInfo.isStringValue()) {
          ecFullName = PathParser::executionContextFullName(opInfo.stringValue());
          ecStateName = PathParser::executionContextStateName(opInfo.stringValue());
        } else {
          ecFullName = getStringValue(opInfo["fullName"], "");
          ecStateName = getStringValue(opInfo["state"], "started");
        }
        
        const auto ecProxy = store.get<OperationAPI>(ecFullName + ":" + "activate_state_" + ecStateName + ".ope");
        if (!ecProxy->isNull()) {
          auto result = juiz::connect(coreBroker(store), name, ecProxy->inlet("__event__"), stateOp->outlet(), option);
          if (result.isError()) {
            logger::error("ProcessBuilder::preStartFSMs() failed. juiz::connect returns error. {}", result);
          }
          return;
        }
        logger::error("ProcessBuilder::preStartFSMs failed. Can not create connection from {} to {}", fsm->fullName(), opInfo);
      });
    });
  });
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
  config["connections"].const_list_for_each([&store](auto& value) {
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
    logger::error("ProcessBuilder::preloadTopics(). ValueTypeException:{}", e.what());
  } 
  logger::trace("ProcessBuilder::preloadTopics() exit");

}

void ProcessBuilder::preloadCallbacksOnStarted(ProcessStore& store, const Value& config, const std::string& path) {
  try {
    auto c = config.at("callbacks");
    c.const_list_for_each([&store](auto& value) {
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