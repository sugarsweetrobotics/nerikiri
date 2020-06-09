#include "nerikiri/nerikiri.h"
#include "nerikiri/logger.h"
#include "nerikiri/connectionbuilder.h"
#include "nerikiri/objectfactory.h"




using namespace nerikiri;

/**
 * もしConnectionInfoで指定されたBrokerが引数のbrokerでなければ，親プロセスに対して別のブローカーをリクエストする
 */
Value checkProcessHasProviderOperation(ProcessStore* store, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;    
    if (store->getOperation(ci.at("output").at("info"))->isNull()) {
      return Value::error(logger::warn("The broker received makeConnection does not have the provider operation."));
    }
    return ci;
}


static std::shared_ptr<OperationBase> _getOperationOrTopic(ProcessStore* store, const Value& info) {
  return store->getOperationOrTopic(info);
}

Value ConnectionBuilder::_validateConnectionInfo(std::shared_ptr<OperationBase> op, const Value& conInfo) {
  auto ret = conInfo;
  if (op->hasOutputConnectionRoute(ret)) {
    return Value::error(logger::warn("makeConnection failed. Provider already have the same connection route {}", str(ret.at("output"))));
  }
  int i = 2;
  while (op->hasOutputConnectionName(ret)) {
      logger::warn("makeConnection failed. Provider already have the same connection name {}", str(ret.at("output")));
      ret["name"] = conInfo.at("name").stringValue() + "_" + std::to_string(i);
      ++i;
  }
  return ret;
}


Value ConnectionBuilder::registerConsumerConnection(ProcessStore* store, const Value& ci) {
  logger::trace("Process::registerConsumerConnection({}", (ci));
  auto ret = ConnectionBuilder::_validateConnectionInfo(_getOperationOrTopic(store, ci.at("input").at("info")), ci);
  return _getOperationOrTopic(store, ci.at("input").at("info"))->addConsumerConnection(consumerConnection(ret,
            ObjectFactory::createBrokerProxy(*store, ci.at("input").at("broker"))));
}



Value ConnectionBuilder::registerProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker/*=nullptr*/) {
  logger::trace("Process::registerProviderConnection({})", (ci));
  try {
    auto ret = ConnectionBuilder::_validateConnectionInfo(_getOperationOrTopic(store, ci.at("output").at("info")), ci);
    auto ret2 = ObjectFactory::createBrokerProxy(*store, ret.at("input").at("broker"))->registerConsumerConnection(ret);
    // リクエストが成功なら、こちらもConnectionを登録。
    auto ret3 = _getOperationOrTopic(store, ci.at("output").at("info"))->addProviderConnection(providerConnection(ret2, ObjectFactory::createBrokerProxy(*store, ret2.at("input").at("broker"))));
    if (ret3.isError()) {
        // 登録が失敗ならConsumer側のConnectionを破棄。
        auto ret3 = deleteConsumerConnection(store, ret2);
        return Value::error(logger::error("request registerProviderConnection for provider's broker failed. ", ret2.getErrorMessage()));
    }// 登録成功ならciを返す
    return ret3;
  } catch (ValueTypeError& ex) {
    return Value::error(logger::error("Process::registerProviderConnection() failed. Exception: " + std::string(ex.what())));
  }
}

  
Value ConnectionBuilder::deleteConsumerConnection(ProcessStore* store, const Value& ci) {
  logger::trace("Process::deleteConsumerConnection({}", (ci));
  return store->getOperation(ci.at("input").at("info"))->removeConsumerConnection(ci);
}

Value ConnectionBuilder::deleteProviderConnection(ProcessStore* store, const Value& ci) {
    logger::trace("Process::deleteProviderConnection({}", (ci));
    if (ci.isError()) return ci;

    auto provider = store->getOperation(ci.at("output").at("info"));
    const auto& connection = provider->getOutputConnection(ci);

    auto ret = connection.info();
    //auto consumerBroker = createBrokerProxy(ret.at("input").at("broker"));
    auto consumerBroker = ObjectFactory::createBrokerProxy(*store, ret.at("input").at("broker"));
    if (!consumerBroker) {
      return Value::error(logger::error("Process::deleteProviderConnection failed. ConsumerBrokerProxy cannot be created."));
    }
    ret = consumerBroker->removeConsumerConnection(ret);
    if (ret.isError()) {
      return Value::error(logger::error("Process::deleteProviderConnection failed. RemoveConsumerConnection failed."));
    }
    return provider->removeProviderConnection(ret);
}
    
Value ConnectionBuilder::registerTopicPublisher(ProcessStore* store, const Value& opInfo, const Value& topicInfo) {
  if (!topicInfo.isError() && !opInfo.isError()) {
    auto instanceName = opInfo.at("instanceName").stringValue();
    if (opInfo.hasKey("ownerContainerInstanceName")) {
      instanceName = opInfo.at("ownerContainerInstanceName").stringValue() + ":" + instanceName;
    }
    return registerProviderConnection(store, {
      {
        {"name", "topic_connection01"},
        {"type", "event"},
        {"input", {
          {"info", {
            {"topicType", "BasicTopic"},
            {"instanceName", topicInfo.at("instanceName")},
            
          }},
          {"broker", {
              {"name", "CoreBroker"}
          }},
          {"target", {{"name", "data"}}}
        }},
        {"output", {
          {"info", { 
            {"instanceName", instanceName},
            {"broker", {
              {"name", "CoreBroker"}
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
    auto instanceName = opInfo.at("instanceName").stringValue();
    if (opInfo.hasKey("ownerContainerInstanceName")) {
      instanceName = opInfo.at("ownerContainerInstanceName").stringValue() + ":" + instanceName;
    }
    return registerProviderConnection(store, {
      {
        {"name", "topic_connection01"},
        {"type", "event"},
        {"input", {
          {"info", { 
            {"instanceName", instanceName}
          }},
          {"broker", {
            {"name", "CoreBroker"}
          }},
          {"target", {{"name", argName}}}
        }},
        {"output", {
          {"info", {
            {"instanceName", topicInfo.at("instanceName")},
            {"topicType", "BasicTopic"},
            {"broker", {
              {"name", "CoreBroker"}
            }}
          }}
        }}
      }
    });
  }
  return Value::error(logger::error("Register Topic Subscriber failed: {}", topicInfo));
}