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
    return store_->getContainer(value).info();
}

Value Broker::getContainerOperationInfos(const Value& info) const {
    return store_->getContainer(info).getOperationInfos();
}

Value Broker::getContainerOperationInfo(const Value& cinfo, const Value& oinfo) const {
    return store_->getContainer(cinfo).getOperation(oinfo).getContainerOperationInfo();
}

Value Broker::getOperationInfo(const Value& info) const {
    return store_->getOperation(info).info();
}

Value Broker::callContainerOperation(const Value& cinfo, const Value& oinfo, Value&& arg) const {
    return store_->getContainer(cinfo).getOperation(oinfo).call(std::move(arg));
}

Value Broker::invokeContainerOperation(const Value& cinfo, const Value& oinfo) const {
    return store_->getContainer(cinfo).getOperation(oinfo).invoke();
}

Value Broker::callOperation(const Value& info, Value&& value) const {
    return store_->getOperation(info).call(std::move(value));
}

Value Broker::invokeOperation(const Value& v) const {
    return store_->getOperation(v).invoke();
}

Value nerikiri::checkDuplicateConsumerConnection(const Broker* broker, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;
    // 同じコネクションを持っていないか確認
    if (broker->store_->getOperation(ci.at("output").at("info")).hasInputConnectionName(ci)) {
        return Value::error(logger::warn("makeConnection failed. Provider already have the same connection {}", str(ci.at("output"))));
    }
    return ci;
}

Value nerikiri::registerConsumerConnection(const Broker_ptr broker, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;
    if (!broker) {
      return Value::error(logger::error("makeConnection failed. The broker does not have the broker proxy ", str(ci.at("output").at("broker"))));
    }
    // ConsumerにregisterConnectionをリクエスト
    return broker->registerConsumerConnection(ci);
}


Value nerikiri::registerProviderConnection(const Broker* broker, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;
    // リクエストが成功なら、こちらもConnectionを登録。
    auto& provider = broker->store_->getOperation(ci.at("output").at("info"));
    auto retval2 = provider.addProviderConnection(providerConnection(ci, broker->process_->createBrokerProxy(ci.at("input").at("broker"))));
    if (retval2.isError()) {
        // 登録が失敗ならConsumer側のConnectionを破棄。
        auto ret = nerikiri::removeConsumerConnection(broker->process_->createBrokerProxy(ci.at("input").at("broker")), ci);
        return Value::error(logger::error("request registerProviderConnection for provider's broker failed. ", retval2.getErrorMessage()));
    }// 登録成功ならciを返す
    return ci;
}

Value nerikiri::removeConsumerConnection(const Broker_ptr broker, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;
    if (!broker) {
      return Value::error(logger::error("makeConnection failed. The broker does not have the broker proxy ", str(ci.at("output").at("broker"))));
    }
    return broker->removeConsumerConnection(ci);
}
/*
Value nerikiri::makeConnection(BrokerAPI* broker, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;
    if (!broker) {
        return Value::error(logger::error("makeConnection failed. The broker does not have the broker proxy ", str(ci.at("output").at("broker"))));
    }
    return broker->makeConnection(ci);
}
*/
Value Broker::getConnectionInfos() const {
    return store_->getConnectionInfos();
}

Value requestConnection(const Value& providerInfo, const Value& connectionInfo) {

}

Value Broker::registerConsumerConnection(const ConnectionInfo& ci) {
    logger::trace("Broker::registerConsumerConnection({}", str(ci));
    return this->process_->registerConsumerConnection(ci);
}

Value Broker::registerProviderConnection(const Value& ci) {
    logger::trace("Broker::registerProviderConnection({}", str(ci));
    return this->process_->registerProviderConnection(ci);
}

Value Broker::removeConsumerConnection(const ConnectionInfo& ci) {
    logger::trace("Broker::removeConsumerConnection({}", str(ci));
    return this->process_->deleteConsumerConnection(ci);
}

Value Broker::removeProviderConnection(const ConnectionInfo& ci) {
    logger::trace("Broker::removeProviderConnection({}", str(ci));
    return this->process_->deleteProviderConnection(ci);
}

Value Broker::putToArgument(const Value& opInfo, const std::string& argName, const Value& value) {
    logger::trace("Broker::putToArgument()");
    return this->process_->putToArgument(opInfo, argName, value);
}

Value Broker::putToArgumentViaConnection(const Value& conInfo, const Value& value) {
    logger::trace("Broker::putToArgumentViaConnection()");
    return this->process_->putToArgumentViaConnection(conInfo, value);
}

Value Broker::requestResource(const std::string& path) const {
    return process_->requestResource(path);//
}

Value Broker::createResource(const std::string& path, const Value& value) {
    return process_->createResource(path, value);
}


Value Broker::deleteResource(const std::string& path) {
    return process_->deleteResource(path);
}