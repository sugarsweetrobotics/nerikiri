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

Value ConnectionBuilder::_validateConnectionInfo(std::shared_ptr<OperationBase> op, const Value& conInfo) {
  auto ret = conInfo;
  if (op->hasOutputConnectionRoute(ret)) {
    return Value::error(logger::warn("ConnectionBuilder::_validateConnectionInfo failed. Provider already have the same connection route {}", str(ret.at("output"))));
  }
  int i = 2;
  while (op->hasOutputConnectionName(ret)) {
      logger::warn("In ConnectionBuilder::_validateConnectionInfo, Provider already have the same connection name {}", str(ret.at("output")));
      ret["name"] = conInfo.at("name").stringValue() + "_" + std::to_string(i);
      ++i;
  }
  return ret;
}


Value ConnectionBuilder::registerConsumerConnection(ProcessStore* store, const Value& ci) {
  logger::trace("Process::registerConsumerConnection({}", (ci));
  auto fullName = ci.at("input").at("info").at("fullName").stringValue();
  auto ret = ConnectionBuilder::_validateConnectionInfo(store->getOperationOrTopic(fullName), ci);

  return store->getOperationOrTopic(fullName)->addConsumerConnection(consumerConnection(ret,
            ObjectFactory::createBrokerProxy(*store, ci.at("input").at("broker"))));
}



Value ConnectionBuilder::registerProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker/*=nullptr*/) {
  logger::trace("ConnectionBuilder::registerProviderConnection({})", (ci));
  try {
    auto outputFullName = ci.at("output").at("info").at("fullName").stringValue();
    auto inputFullName = ci.at("input").at("info").at("fullName").stringValue();
    auto conName = ci.at("name").stringValue();
    auto argName = ci.at("target").at("name").stringValue();

    auto ret = ConnectionBuilder::_validateConnectionInfo(store->getOperationOrTopic(outputFullName), ci);
    auto inputBroker = ObjectFactory::createBrokerProxy(*store, ci.at("input").at("broker"));
    auto ret2 = inputBroker->registerConsumerConnection(ret);
    // リクエストが成功なら、こちらもConnectionを登録。
    auto ret3 = store->getOperationOrTopic(outputFullName)->addProviderConnection(providerConnection(ret2, inputBroker));
    if (ret3.isError()) {
        // 登録が失敗ならConsumer側のConnectionを破棄。
        if (ret2.isError()) {
            auto consumerConName = ret2.at("name").stringValue();
            inputBroker->removeConsumerConnection(inputFullName, argName, consumerConName);
        }
        return Value::error(logger::error("request registerProviderConnection for provider's broker failed. ", ret2.getErrorMessage()));
    }// 登録成功ならciを返す
    return ret3;
  } catch (ValueTypeError& ex) {
    return Value::error(logger::error("ConnectionBuilder::registerProviderConnection(" + str(ci) + ") failed. Exception: " + std::string(ex.what())));
  }
}

  
Value ConnectionBuilder::deleteConsumerConnection(ProcessStore* store, const std::string& fullName, const std::string& targetArgName, const std::string& conName) {
  logger::trace("Process::deleteConsumerConnection({}", fullName);
  return store->getOperation(fullName)->removeConsumerConnection(targetArgName, conName);
}

Value ConnectionBuilder::deleteProviderConnection(ProcessStore* store, const std::string& fullName, const std::string& conName) {
    logger::trace("Process::deleteProviderConnection({}, {})", fullName, conName);
    
    auto provider = store->getOperation(fullName);
    const auto& connection = provider->getOutputConnection(conName);
    if (connection.isNull()) {
      return Value::error(logger::error("ConnectionBuilder::deleteProviderConnection failed. ProviderOperation can not find connection named(" + conName + ")"));
    }
    auto ret = connection.info();
    auto consumerName = connection.info().at("input").at("info").at("fullName").stringValue();
    auto targetArgName = connection.info().at("target").at("name").stringValue();

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