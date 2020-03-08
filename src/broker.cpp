#include "nerikiri/broker.h"

#include "nerikiri/process.h"
#include "nerikiri/connection.h"

using namespace nerikiri;

Broker_ptr Broker::null = Broker_ptr(nullptr);

Value Broker::getProcessInfo() const{
    return process_->info();
}

Value Broker::getProcessOperationInfos() const {
    return process_->getOperationInfos();
}

Value Broker::getOperationInfoByName(const std::string& name) const {
    OperationInfo info;
    process_->getOperationInfos().list_for_each([&name, &info](auto& oi) {
        if (oi["name"].stringValue() == name) {
            info = oi;
        }
    });
    return info;
}

Value Broker::callOperationByName(const std::string& name, Value&& value) const {
    return nerikiri::call_operation(process_->getOperationByName(name), std::move(value));
}

Value Broker::invokeOperationByName(const std::string& name) const {
    return process_->invokeOperationByName(name);
}

Value Broker::makeConnection(const ConnectionInfo& ci) const {
    logger::trace("Broker::makeConnection({}", str(ci));
    // まず、もし出力側 (Provider) 出なければProvider側にmakeConnectionを連鎖する
    auto& provider = process_->getOperationByInfo(ci.at("provider").at("process"));
    if (provider.isNull()) {
      logger::warn("The broker received makeConnection does not have the provider operation.");
      // Provider側のBrokerProxyを取得
      auto broker = process_->getBrokerByInfo(ci.at("provider").at("broker"));
      // BrokerProxyにmakeConnectionを送ってそのままリターン
      if (broker) {
          return broker->makeConnection(ci);
      } 
      std::stringstream ss;
      ss << "makeConnection failed. The broker does not have the broker proxy " << str(ci.at("provider").at("broker"));
      logger::warn(ss.str().c_str());
      return Value::error(ss.str().c_str());
    }

    // 自分はprovider側。

    // Consumer側との接続確認
    // Consumer側のBrokerProxyを取得
    auto broker = process_->getBrokerByInfo(ci.at("consumer").at("broker"));    
    if (!broker) {
      std::stringstream ss;
      ss << "makeConnection failed. The broker does not have the broker proxy " << str(ci.at("provider").at("broker"));
      logger::error(ss.str().c_str());
      return Value::error(ss.str());
    }
    // ConsumerにregisterConnectionをリクエスト
    auto retval = broker->registerConsumerConnection(ci);
    if (retval.isError()) {
        std::stringstream ss;
        ss << "request registerConsumerConnection for consumer's broker failed. " << retval.getErrorMessage();
        logger::error(ss.str().c_str());
        return Value::error(ss.str().c_str());
    }


    ///provider = process_->getOperationByInfo(ci.at("provider").at("process"));
    // リクエストが成功なら、こちらもConnectionを登録。
    if (provider.hasConsumerConnection(ci)) {
        std::stringstream ss;
        ss << "makeConnection failed. Provider already have the same connection." << str(ci.at("provider"));
        logger::warn(ss.str().c_str());

        // 登録が失敗ならConsumer側のConnectionを破棄。
        auto retval3 = broker->removeConsumerConnection(ci);
        if (retval3.isError()) {
            std::stringstream ss;
            ss << "request removeConsumerConnection for consumer's broker failed. " << retval3.getErrorMessage();
            logger::error(ss.str().c_str());
            return Value::error(ss.str().c_str());
        }
        return Value::error(ss.str());
    }

    //provider = process_->getOperationByInfo(ci.at("provider").at("process"));
    auto retval2 = provider.addProviderConnection(Connection(ci, process_->getBrokerByInfo(ci.at("consumer").at("broker"))));
    if (!retval2.isError()) {
        // 登録成功ならciを返す
        return ci;
    }

    // 登録が失敗ならConsumer側のConnectionを破棄。
    auto retval3 = broker->removeConsumerConnection(ci);
    if (retval3.isError()) {
        std::stringstream ss;
        ss << "request removeConsumerConnection for consumer's broker failed. " << retval3.getErrorMessage();
        logger::error(ss.str().c_str());
        return Value::error(ss.str());
    }

    std::stringstream ss;
    ss << "request registerProviderConnection for provider's broker failed. " << retval2.getErrorMessage();
    logger::error(ss.str());
    return Value::error(ss.str());
}

Value Broker::registerConsumerConnection(const ConnectionInfo& ci) const {
    logger::trace("Broker::registerConsumerConnection({}", str(ci));
    /// Consumer側でなければ失敗出力
    auto& consumer = process_->getOperationByInfo(ci.at("consumer").at("process"));
    if (consumer.isNull()) {
        std::stringstream ss;
        ss << "registerConsumerConnection failed. The broker does not have the consumer " << str(ci.at("consumer"));
        logger::warn(ss.str());
        return Value::error(ss.str());
    }

    if (consumer.hasConsumerConnection(ci)) {
        std::stringstream ss;
        ss << "registerConsumerConnection failed. Consumer already have the same connection." << str(ci.at("consumer"));
        logger::warn(ss.str());
        return Value::error(ss.str());
    }
    return consumer.addConsumerConnection(Connection(ci, process_->getBrokerByInfo(ci.at("provider").at("broker"))));
}


Value Broker::removeConsumerConnection(const ConnectionInfo& ci) const {
    logger::trace("Broker::removeConsumerConnection({}", str(ci));
    /// Consumer側でなければ失敗出力
    auto& consumer = process_->getOperationByInfo(ci.at("consumer"));
    if (consumer.isNull()) {
        std::stringstream ss;
        ss << "removeConsumerConnection failed. The broker does not have the consumer " << str(ci.at("consumer"));
        logger::warn(ss.str());
        return Value::error(ss.str());
    }

    if (!consumer.hasConsumerConnection(ci)) {
        std::stringstream ss;
        ss << "removeConsumerConnection failed. Consumer does not have the same connection." << str(ci.at("consumer"));
        logger::warn(ss.str());
        return Value::error(ss.str());
    }
    return consumer.removeConsumerConnection(ci);
}

