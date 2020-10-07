#include "nerikiri/nerikiri.h"
#include "nerikiri/logger.h"
#include "nerikiri/connectionbuilder.h"
#include "nerikiri/objectfactory.h"




using namespace nerikiri;

/**
 * もしConnectionInfoで指定されたBrokerが引数のbrokerでなければ，親プロセスに対して別のブローカーをリクエストする
 */
//static bool doProcessHasProviderOperation(ProcessStore* store, const std::string& fullName) {
//    return store->getOperation(fullName)->isNull();/
//}


//static std::shared_ptr<OperationBase> _getOperationOrTopic(ProcessStore* store, const Value& info) {
//  return store->getOperationOrTopic(info.at("fullName").stringValue());
//}

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

Value ConnectionBuilder::createConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker/*=nullptr*/) {
  auto type = getStringValue(ci.at("type"), "");
  if (type == "stateBind") {
    return ConnectionBuilder::bindOperationToFSM(store, ci, receiverBroker);
  }
  return ConnectionBuilder::registerProviderConnection(store, ci, receiverBroker);
}

Value ConnectionBuilder::deleteConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker/*=nullptr*/) {
  return Value::error(logger::error("ConnectionBuilder::deleteConnection(" + str(ci) + ") failed."));
}

Value ConnectionBuilder::bindOperationToFSM(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker/*=nullptr*/) {
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

Value ConnectionBuilder::unbindOperationToFSM(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker/*=nullptr*/) {

}

Value ConnectionBuilder::registerProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker/*=nullptr*/) {
  logger::trace("ConnectionBuilder::registerOperationProviderConnection({})", ci);
  return ConnectionBuilder::registerOperationProviderConnection(store, ci, receiverBroker);
}

Value ConnectionBuilder::registerOperationProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker/*=nullptr*/) {
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
    
Value ConnectionBuilder::registerTopicPublisher(ProcessStore* store, const Value& opInfo, const Value& topicInfo) {
  logger::trace("ConnectionBuilder::registerTopicPublisher({}, {})", opInfo, topicInfo);
  if (!topicInfo.isError() && !opInfo.isError()) {
    auto fullName = opInfo.at("fullName").stringValue();
    //if (opInfo.hasKey("ownerContainerInstanceName")) {
    //  instanceName = opInfo.at("ownerContainerInstanceName").stringValue() + ":" + instanceName;
    //}
    return registerProviderConnection(store, {
      {
        {"name", "topic_connection01"},
        {"type", "event"},
        {"input", {
          {"info", {
            {"topicType", "BasicTopic"},
            {"fullName", topicInfo.at("fullName")},
            
          }},
          {"broker", {
              {"typeName", "CoreBroker"}
          }},
          {"target", {{"name", "data"}}}
        }},
        {"output", {
          {"info", { 
            {"fullName", fullName},
            {"broker", {
              {"typeName", "CoreBroker"}
            }}
          }}
        }}
      }
    });
  }
  return Value::error(logger::error("Register Topic Publisher failed: {}", topicInfo));
}

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