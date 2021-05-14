
#include "process_builder.h"
#include "moduleloader.h"
#include <nerikiri/objectfactory.h>
#include "nerikiri/connection/connection_builder.h"
#include "nerikiri/topic/topic_factory.h"

using namespace nerikiri;
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

  logger::trace("ProcessBuilder::preloadOperations() exit");
}


void ProcessBuilder::preloadContainers(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::preloadContainers(path={}) entry", path);
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

  config.at("ecs").at("bind").const_object_for_each([&store](auto ecName, auto value) {
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
    value.const_list_for_each([&store, &ec, &activate_started_ope](auto& v) {
      auto opProxy =  store.operationProxy(v);
      nerikiri::connect(coreBroker(store), ec->fullName() + "_bind_" + Value::string(v["fullName"]), 
        opProxy->inlet("__event__"), activate_started_ope->outlet(), {});
      //ec->bind(store.operation(Value::string(v.at("fullName"))));
    });
  });
  /*
  
  config.at("ecs").at("precreate").const_list_for_each([&store](auto& value) {
    ObjectFactory::createExecutionContext(store, value);
  });

  
  */
  logger::trace("Process::_preloadExecutionContexts() exit");
}


/**
 * 
 */
void ProcessBuilder::preStartFSMs(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("Process::_preStartFSMs() entry");
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
        nerikiri::connect(coreBroker(store), name, store.operationProxy(opInfo)->inlet("__argument__"), stateOp->outlet(), option);
      });
      bindInfo["ecs"].const_list_for_each([&store, &stateOp](auto& ecInfo) {
        auto name = stateOp->fullName() + "_state_connection_" + Value::string(ecInfo["fullName"]) + "_" + Value::string(ecInfo["state"]) ;
        Value ecStateOpInfo {
          {"fullName", Value::string(ecInfo["fullName"]) + ":activate_state_" + Value::string(ecInfo["state"]) + ".ope"}
        };
        if (ecInfo.hasKey("broker")) {
          ecStateOpInfo["broker"] = ecInfo["broker"];
        }
        nerikiri::connect(coreBroker(store), name, store.operationProxy(ecStateOpInfo)->inlet("__event__"), stateOp->outlet());
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
  //} catch (nerikiri::ValueTypeError& e) {
  //  logger::debug("Process::_preloadFSMs(). ValueTypeException:{}", e.what());
  //}

  logger::trace("Process::_preStartFSMs() exit");
}

void ProcessBuilder::preStartExecutionContexts(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::preStartExecutionContexts() entry");
  nerikiri::getListValue(config, "ecs.start").const_list_for_each([&store](const auto& value) {
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

  return nerikiri::connect(broker, connectionName, topic->inlet("data"), op->outlet());
}


Value ProcessBuilder::subscribeTopic(ProcessStore& store, const std::shared_ptr<ClientProxyAPI>& broker, const Value& opInfo, const std::string& argName, const Value& topicInfo) {
  logger::trace("ProcessBuilder::subscribeTopic({}, {}, {})", opInfo, argName, topicInfo);
  
  auto op = store.get<OperationAPI>(Value::string(opInfo.at("fullName")));
  auto topicInfo2 = ObjectFactory::createTopic(store, topicInfo);
  auto topic = store.get<TopicAPI>(Value::string(topicInfo.at("fullName")));
  auto connectionName = "topic_connection_" + op->fullName() + "_" + argName + "_" + Value::string(topicInfo.at("fullName"));

  return nerikiri::connect(broker, connectionName, op->inlet(argName), topic->outlet());
}

void _parseOperationInfo(ProcessStore& store, const std::shared_ptr<ClientProxyAPI>& broker, const Value& opInfo) {
  logger::trace("{} _parseOperationInfo(info={}) called.", __FILE__, opInfo);
  if (opInfo["publish"].isStringValue()) {
    ProcessBuilder::publishTopic(store, broker, opInfo, {{"fullName", opInfo["publish"]}});
  }
  else {
    opInfo["publish"].const_list_for_each([&opInfo, &store, &broker](auto &topicInfo) {
      if (topicInfo.isStringValue()) {
        ProcessBuilder::publishTopic(store, broker, opInfo, {{"fullName", topicInfo}});
      } else {
        ProcessBuilder::publishTopic(store, broker, opInfo, topicInfo);
      }
    });
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
}

void ProcessBuilder::preloadTopics(ProcessStore& store, const std::shared_ptr<ClientProxyAPI>& broker, const Value& config, const std::string& path) {
  logger::trace("ProcessBuilder::preloadTopics() entry");
  try {
    std::vector<Value> topicInfos;
    config["operations"]["precreate"].const_list_for_each([&store, &broker](auto &opInfo) {
      _parseOperationInfo(store, broker, opInfo);
    });

    config.at("containers").at("precreate").const_list_for_each([&store, &broker](auto &conInfo) {
      conInfo.at("operations").const_list_for_each([&conInfo, &store, &broker](auto &opInfo_) {
        Value opInfo = opInfo_;
        auto fullNameValue = conInfo["fullName"];
        if (!conInfo["fullName"].isStringValue()) {
          fullNameValue = conInfo["instanceName"];
        }
        opInfo["fullName"] = fullNameValue.stringValue() + ":" + opInfo_["instanceName"].stringValue();
        _parseOperationInfo(store, broker, opInfo);
      });
    });

    /*
    
    config.at("containers").at("precreate").const_list_for_each([&store](auto &cInfo) {
      cInfo.at("operations").const_list_for_each([&cInfo, &store](auto &opInfo) {
        opInfo.at("publish").const_list_for_each([&cInfo, &opInfo, &store](auto &topicInfo) {
          ConnectionBuilder::registerTopicPublisher(store, cInfo, opInfo, topicInfo);
        });
        opInfo.at("subscribe").const_list_for_each([](auto &topicInfo) {
          
        });
      });
    });

    auto operationCallback = [&store](const Value& opInfo) -> void {
      logger::trace("Process::_preloadTopics(): operationCallback for opInfo={}", opInfo);
      getListValue(opInfo, "publish").const_list_for_each([&store, &opInfo](auto topicInfo) { 
        ObjectFactory::createTopic(store, topicInfo);
        //ConnectionBuilder::registerTopicPublisher(&store, opInfo, ObjectFactory::createTopic(store,
        //  {{"fullName", v.stringValue()}, {"defaultArg", { {"data", {}} }}} ));
      });
      /*
      opInfo.at("publish").const_list_for_each([this, &opInfo](auto v) {
        ConnectionBuilder::registerTopicPublisher(store(), opInfo, ObjectFactory::createTopic(store_,
         {{"fullName", v.stringValue()}, {"defaultArg", { {"data", {}} }}} ));
      });
      *
      getObjectValue(opInfo, "subscribe").const_object_for_each([&opInfo](auto key, auto v) {
        v.list_for_each([&opInfo, key](auto sv) {
         // ConnectionBuilder::registerTopicSubscriber(store(), opInfo, key, ObjectFactory::createTopic(store_,
         //  {{"fullName", sv.stringValue()}, {"defaultArg", { {"data", {}} }}} ));
        });
      });
      /*
      opInfo.at("subscribe").const_object_for_each([this, &opInfo](auto key, auto v) {
        v.list_for_each([this, &opInfo, key](auto sv) {
          ConnectionBuilder::registerTopicSubscriber(store(), opInfo, key, ObjectFactory::createTopic(store_,
           {{"fullName", sv.stringValue()}, {"defaultArg", { {"data", {}} }}} ));
        });
      });
      *
    };
    
    
    //store()->getOperationInfos().const_list_for_each(operationCallback);
    //for(auto pc : store()->getContainers()) {
    //  for(auto opInfo : pc->getOperationInfos()) {
    //    operationCallback(opInfo);
    //  }
    //}
    **/

  } catch (nerikiri::ValueTypeError& e) {
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