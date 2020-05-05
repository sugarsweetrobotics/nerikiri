#include "nerikiri/nerikiri.h"
#include "nerikiri/util/logger.h"
#include "nerikiri/connectionbuilder.h"
#include "nerikiri/objectfactory.h"




using namespace nerikiri;


Value ConnectionBuilder::registerConsumerConnection(ProcessStore* store, const Value& ci) {

  logger::trace("Process::registerConsumerConnection({}", (ci));
  if (ci.isError()) return ci;

  auto ret = ci;
  /// Consumer側でなければ失敗出力
  auto consumer = store->getOperation(ci.at("input").at("info"));
  if (consumer->isNull()) {
      return Value::error(logger::error("registerConsumerConnection failed. The broker does not have the consumer ", str(ci.at("input"))));
  }
  if (consumer->hasInputConnectionRoute(ret)) {
      return Value::error(logger::error("registerConsumerConnection failed. Consumer already have the same connection.", str(ci.at("input"))));
  }
  while (consumer->hasInputConnectionName(ret)) {
      logger::warn("registerConsumerConnection failed. Consumer already have the same connection.", (ret.at("input")));
      ret["name"] = ret["name"].stringValue() + "_2";
  }

  return consumer->addConsumerConnection(consumerConnection(ret,
             //createBrokerProxy(ci.at("input").at("broker"))));
            ObjectFactory::createBrokerProxy(*store, ci.at("input").at("broker"))));
}


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


Value ConnectionBuilder::registerProviderConnection(ProcessStore* store, const Value& ci, BrokerAPI* receiverBroker/*=nullptr*/) {
  logger::trace("Process::registerProviderConnection({})", (ci));
  try {
    auto ret = checkProcessHasProviderOperation(store, ci);
    if (ret.isError()) return ret;
    auto provider = store->getOperation(ret.at("output").at("info"));
    if (provider->hasOutputConnectionRoute(ci)) {
        return Value::error(logger::warn("makeConnection failed. Provider already have the same connection route {}", str(ci.at("output"))));
    }
    while (provider->hasOutputConnectionName(ret)) {
        logger::warn("makeConnection failed. Provider already have the same connection route {}", str(ret.at("output")));
        ret["name"] = ci.at("name").stringValue() + "_2";
    }

//    auto consumerBroker = createBrokerProxy(ret.at("input").at("broker"));
      auto consumerBroker = ObjectFactory::createBrokerProxy(*store, ret.at("input").at("broker"));
    if (!consumerBroker) {
      return Value::error(logger::error("makeConnection failed. Consumer side broker can not be created. {}", str(ci.at("output"))));
    }

    auto ret2 = consumerBroker->registerConsumerConnection(ret);
    if(ret2.isError()) return ret;

    // リクエストが成功なら、こちらもConnectionを登録。
    //auto ret3 = provider->addProviderConnection(providerConnection(ret2, createBrokerProxy(ret2.at("input").at("broker"))));
    auto ret3 = provider->addProviderConnection(providerConnection(ret2, ObjectFactory::createBrokerProxy(*store, ret2.at("input").at("broker"))));
    if (ret3.isError()) {
        // 登録が失敗ならConsumer側のConnectionを破棄。
        auto ret3 = deleteConsumerConnection(store, ret2);
        return Value::error(logger::error("request registerProviderConnection for provider's broker failed. ", ret2.getErrorMessage()));
    }// 登録成功ならciを返す
    return ret2;
  } catch (ValueTypeError& ex) {
    return Value::error(logger::error("Process::registerProviderConnection() failed. Exception: {}", ex.what()));
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
    