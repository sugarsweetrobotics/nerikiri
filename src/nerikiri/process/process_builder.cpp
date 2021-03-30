
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
  logger::trace("Process::_preloadOperations() entry");
  config.at("operations").at("preload").const_list_for_each([&store, &config, &path](auto value) {
    ModuleLoader::loadOperationFactory(store, {"./", path}, {
      {"typeName", value}, {"load_paths", config.at("operations").at("load_paths")}
    });
  });

  config.at("operations").at("precreate").const_list_for_each([&store](auto& oinfo) {
    auto opInfo = ObjectFactory::createOperation(store, oinfo);
    auto op = store.operation(Value::string(opInfo.at("fullName")));
    if (oinfo.hasKey("publish")) {
      oinfo.at("publish").const_list_for_each([&op, &store](auto pubTopicInfo) {
        TopicFactory::publishTopic(store, op, pubTopicInfo);
      });
    }
  });

  logger::trace("Process::_preloadOperations() exit");
}


void ProcessBuilder::preloadContainers(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("Process::_preloadContainers() entry");
  config.at("containers").at("preload").const_list_for_each([&store, &config, &path](auto value) {
    ModuleLoader::loadContainerFactory(store, {"./", path}, {
      {"typeName", value.at("typeName")}, {"load_paths", config.at("containers").at("load_paths")}
    });
    if (value.hasKey("operations")) {
      value.at("operations").const_list_for_each([&config, &store, &value, &path](auto& v) {
        ModuleLoader::loadContainerOperationFactory(store, {"./", path}, {
          {"typeName", v}, {"container_name", value.at("typeName")}, {"load_paths", config.at("containers").hasKey("load_paths")}
        });
      });
    }
  });
  
  config.at("containers").at("precreate").const_list_for_each([&store](auto info) {
    auto cInfo = ObjectFactory::createContainer(store, info);
    if (info.hasKey("operations")) {
      info.at("operations").const_list_for_each([&store, &cInfo](auto value) {
        auto opInfo = ObjectFactory::createContainerOperation(store, cInfo, value);

        auto c = store.container(Value::string(cInfo.at("fullName")));
        auto cop = c->operation(Value::string(opInfo.at("fullName")));
        if (value.hasKey("publish")) {
          value.at("publish").const_list_for_each([&store, &cop](auto topicInfo) {
            //ConnectionBuilder::createConnection(cop, ObjectFactory::createTopic(store, topicInfo));
            // TODO: クリエイトコネクション
          });
        }
        if (value.hasKey("subscribe")) {
        }
      });
    }
  });

  logger::trace("Process::_preloadContainers() exit");
}

void ProcessBuilder::preloadFSMs(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("Process::_preloadFSMs() entry");
  config.at("fsms").at("precreate").const_list_for_each([&store](auto& value) {
    ObjectFactory::createFSM(store, value);
  });
  logger::trace("Process::_preloadFSMs() exit");
}


void ProcessBuilder::preloadExecutionContexts(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("Process::_preloadExecutionContexts() entry");
  config.at("ecs").at("preload").const_list_for_each([&store, &config, &path](auto& value) {
    ModuleLoader::loadExecutionContextFactory(store, {"./", path}, {
      {"typeName", value}, {"load_paths", config.at("ecs").at("load_paths")}
    });
  });
  
  config.at("ecs").at("precreate").const_list_for_each([&store](auto& value) {
    ObjectFactory::createExecutionContext(store, value);
  });

  config.at("ecs").at("bind").const_object_for_each([&store](auto key, auto value) {
    value.const_list_for_each([&store, &key](auto& v) {
      store.executionContext(key)->bind(store.operation(Value::string(v.at("fullName"))));
    });
  });
  
  logger::trace("Process::_preloadExecutionContexts() exit");
}


/**
 * 
 */
void ProcessBuilder::preStartFSMs(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("Process::_preStartFSMs() entry");
  
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
      */
    });
    auto c = config.at("fsms").at("bind");
    auto ecs = c.at("ecs");
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
  logger::trace("Process::_preStartExecutionContexts() entry");
  nerikiri::getListValue(config, "ecs.start").const_list_for_each([&store](const auto& value) {
    store.executionContext(value.stringValue())->start();
  });
    /*
    auto c = config_.at("ecs").at("start");
    c.list_for_each([this](auto& value) {
      this->store()->getExecutionContext(value.stringValue())->start();
    });
    */
  logger::trace("Process::_preStartExecutionContexts() exit");
}

void ProcessBuilder::preloadBrokers(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("Process::_preloadBrokers() entry");
  config.at("brokers").at("preload").const_list_for_each([&store, &config, &path](auto& value) {
    ModuleLoader::loadBrokerFactory(store, {"./", path}, {
      {"typeName", value}, {"load_paths", config.at("brokers").at("load_paths")}
    });
  });
  config.at("brokers").at("precreate").const_list_for_each([&store](auto& value) {
    ObjectFactory::createBroker(store, value);
  });

  logger::trace("Process::_preloadBrokers() exit");
}

void ProcessBuilder::preloadConnections(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("Process::_preloadConnections() entry");
  config.at("connections").const_list_for_each([&store](auto& value) {
   // ConnectionBuilder::registerProviderConnection(store(), value);
    ConnectionBuilder::createConnection(&store, value);
  });
  logger::trace("Process::_preloadConnections() exit");
}


void ProcessBuilder::preloadTopics(ProcessStore& store, const Value& config, const std::string& path) {
  logger::trace("Process::_preloadTopics() entry");
  try {
    /*
    std::vector<Value> topicInfos;
    config.at("operations").at("precreate").const_list_for_each([&store](auto &opInfo) {
      opInfo.at("publish").const_list_for_each([&opInfo, &store](auto &topicInfo) {
        ConnectionBuilder::registerTopicPublisher(store, opInfo, topicInfo);
      });
      opInfo.at("subscribe").const_list_for_each([&store](auto &topicInfo) {
        
      });
    });
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
    logger::debug("Process::_preloadTopics(). ValueTypeException:{}", e.what());
  } 
  logger::trace("Process::_preloadTopics() exit");

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
          store.operation(opName)->call(argument);
        });
      }
    });
  } catch (ValueTypeError& ex) {
    logger::error("Process::_preloadCallbacksOnStarted failed. Exception: {}", ex.what());
  }
}