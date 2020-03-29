#include "nerikiri/broker.h"

#include "nerikiri/process.h"
#include "nerikiri/operation.h"
#include "nerikiri/connection.h"
#include "nerikiri/objectmapper.h"

using namespace nerikiri;

Broker_ptr Broker::null = Broker_ptr(nullptr);

Value Broker::getProcessInfo() const{ return process_->info(); }

Value Broker::getOperationInfos() const {
    return store_->getOperationInfos();
}

Value Broker::getContainerInfos() const {
    return store_->getContainerInfos();
}

Value Broker::getContainerInfo(const Value& value) const {
    return store_->getContainerByName(value.at("name").stringValue()).info();
}

Value Broker::getContainerOperationInfos(const Value& info) const {
    return store_->getContainerByName(info.at("name").stringValue()).getOperationInfos();
}

Value Broker::getContainerOperationInfo(const Value& cinfo, const Value& oinfo) const {
    return store_->getContainerByName(cinfo.at("name").stringValue()).getOperation(oinfo).getContainerOperationInfo();
}

Value Broker::getOperationInfo(const Value& info) const {
    return store_->getOperation(info).info();
}

Value Broker::callContainerOperation(const Value& cinfo, const Value& oinfo, Value&& arg) const {
    return nerikiri::call_operation(store_->getContainerByName(cinfo.at("name").stringValue()).getOperation(oinfo), std::move(arg));
}

Value Broker::invokeContainerOperation(const Value& cinfo, const Value& oinfo) const {
    return nerikiri::invoke_operation(store_->getContainerByName(cinfo.at("name").stringValue()).getOperation(oinfo));
}

Value Broker::callOperation(const Value& info, Value&& value) const {
    return nerikiri::call_operation(store_->getOperation(info), std::move(value));
}

Value Broker::invokeOperationByName(const std::string& name) const {
    return nerikiri::invoke_operation(store_->getOperation({{"name", name}}));
}

/**
 * もしConnectionInfoで指定されたBrokerが引数のbrokerでなければ，親プロセスに対して別のブローカーをリクエストする
 */
Value nerikiri::relayProvider(const Broker* broker, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;
    // まず、もし出力側 (Provider) 出なければProvider側にmakeConnectionを連鎖する
    
    if (broker->store_->getOperation(ci.at("provider").at("info")).isNull()) {
      logger::warn("The broker received makeConnection does not have the provider operation.");
      // Provider側のBrokerProxyを取得
      
      return makeConnection(broker->process_->getBrokerByInfo(ci.at("provider").at("broker")).get(), ci);
    }
    return ci;
}

Value nerikiri::checkDuplicateConsumerConnection(const Broker* broker, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;
    // 同じコネクションを持っていないか確認
    if (broker->store_->getOperation(ci.at("provider").at("info")).hasConsumerConnection(ci)) {
        return Value::error(logger::warn("makeConnection failed. Provider already have the same connection {}", str(ci.at("provider"))));
    }
    return ci;
}

Value nerikiri::registerConsumerConnection(const Broker_ptr broker, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;
    if (!broker) {
      return Value::error(logger::error("makeConnection failed. The broker does not have the broker proxy ", str(ci.at("provider").at("broker"))));
    }
    // ConsumerにregisterConnectionをリクエスト
    return broker->registerConsumerConnection(ci);
}

Value nerikiri::registerProviderConnection(const Broker* broker, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;
    // リクエストが成功なら、こちらもConnectionを登録。
    auto& provider = broker->store_->getOperation(ci.at("provider").at("info"));
    auto retval2 = provider.addProviderConnection(providerConnection(ci, broker->process_->getBrokerByInfo(ci.at("consumer").at("broker"))));
    if (retval2.isError()) {
        // 登録が失敗ならConsumer側のConnectionを破棄。
        auto ret = nerikiri::removeConsumerConnection(broker->process_->getBrokerByInfo(ci.at("consumer").at("broker")), ci);
        return Value::error(logger::error("request registerProviderConnection for provider's broker failed. ", retval2.getErrorMessage()));
    }// 登録成功ならciを返す
    return ci;
}

Value nerikiri::removeConsumerConnection(const Broker_ptr broker, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;
    if (!broker) {
      return Value::error(logger::error("makeConnection failed. The broker does not have the broker proxy ", str(ci.at("provider").at("broker"))));
    }
    return broker->removeConsumerConnection(ci);
}

Value nerikiri::makeConnection(const BrokerAPI* broker, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;
    if (!broker) {
        return Value::error(logger::error("makeConnection failed. The broker does not have the broker proxy ", str(ci.at("provider").at("broker"))));
    }
    return broker->makeConnection(ci);
}

Value Broker::getConnectionInfos() const {
    return store_->getConnectionInfos();
}

Value Broker::makeConnection(const ConnectionInfo& ci) const {
    if (ci.isError()) return ci;
    logger::trace("Broker::makeConnection({}", str(ci));
    // まず、もし出力側 (Provider) 出なければProvider側にmakeConnectionを連鎖する
   
    auto ret = nerikiri::relayProvider(this, ci);
    
    ret = nerikiri::checkDuplicateConsumerConnection(this, ret);

    // Consumer側との接続確認
    // Consumer側のBrokerProxyを取得
    ret = nerikiri::registerConsumerConnection(process_->getBrokerByInfo(ci.at("consumer").at("broker")), ret);
    
    // ConsumerにregisterConnectionをリクエスト
    
    // リクエストが成功なら、こちらもConnectionを登録。
    ret = nerikiri::registerProviderConnection(this, ret);
    

    return ret;
}

Value Broker::registerConsumerConnection(const ConnectionInfo& ci) const {
    logger::trace("Broker::registerConsumerConnection({}", str(ci));
    /// Consumer側でなければ失敗出力
    auto& consumer = store_->getOperation(ci.at("consumer").at("info"));
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
    return consumer.addConsumerConnection(consumerConnection(ci, process_->getBrokerByInfo(ci.at("provider").at("broker"))));
}


Value Broker::removeConsumerConnection(const ConnectionInfo& ci) const {
    logger::trace("Broker::removeConsumerConnection({}", str(ci));
    /// Consumer側でなければ失敗出力
    auto& consumer = store_->getOperation(ci.at("consumer"));
    if (consumer.isNull()) {
        return Value::error(logger::warn("removeConsumerConnection failed. The broker does not have the consumer ", str(ci.at("consumer"))));
    }

    if (!consumer.hasConsumerConnection(ci)) {
        return Value::error(logger::warn("removeConsumerConnection failed. Consumer does not have the same connection.", str(ci.at("consumer"))));
    }
    return consumer.removeConsumerConnection(ci);
}

Value Broker::pushViaConnection(const ConnectionInfo& ci, Value&& value) const {
    auto& op = store_->getOperation(ci.at("consumer"));
    if (op.isNull()) {
        return Value::error(logger::error("Operation({}) can not be found.", str(ci.at("consumer"))));
    }
    return op.push(ci, std::move(value));
}

Value Broker::requestResource(const std::string& path) const {
    return nerikiri::ObjectMapper::requestResource(store_, path);
}

Value Broker::createResource(const std::string& path, const Value& value) {
    return nerikiri::ObjectMapper::createResource(store_, path, value);
}