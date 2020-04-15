#include "nerikiri/brokers/corebroker.h"

#include "nerikiri/process.h"
#include "nerikiri/operation.h"
#include "nerikiri/connection.h"
#include "nerikiri/objectmapper.h"

using namespace nerikiri;

//std::shared_ptr<BrokerAPI>  Broker::null = std::shared_ptr<BrokerAPI>(nullptr);

Value CoreBroker::getProcessInfo() const{ return process_->info(); }

Value CoreBroker::getOperationInfos() const {
    return process_->store()->getOperationInfos();
}

Value CoreBroker::getContainerInfos() const {
    return process_->store()->getContainerInfos();
}

Value CoreBroker::getContainerInfo(const Value& value) const {
    return process_->store()->getContainer(value)->info();
}

Value CoreBroker::getContainerOperationInfos(const Value& info) const {
    return process_->store()->getContainer(info)->getOperationInfos();
}

Value CoreBroker::getContainerOperationInfo(const Value& cinfo, const Value& oinfo) const {
    return process_->store()->getContainer(cinfo)->getOperation(oinfo)->info();
}

Value CoreBroker::getOperationInfo(const Value& info) const {
    return process_->store()->getOperation(info)->info();
}

Value CoreBroker::callContainerOperation(const Value& cinfo, const Value& oinfo, Value&& arg) const {
    return process_->store()->getContainer(cinfo)->getOperation(oinfo)->call(std::move(arg));
}

Value CoreBroker::invokeContainerOperation(const Value& cinfo, const Value& oinfo) const {
    return process_->store()->getContainer(cinfo)->getOperation(oinfo)->invoke();
}

Value CoreBroker::callOperation(const Value& info, Value&& value) const {
    return process_->store()->getOperation(info)->call(std::move(value));
}

Value CoreBroker::invokeOperation(const Value& v) const {
    return process_->store()->getOperation(v)->invoke();
}

Value CoreBroker::executeOperation(const Value& v) {
    return process_->store()->getOperation(v)->execute();
}


Value CoreBroker::getConnectionInfos() const {
    return process_->store()->getConnectionInfos();
}

Value CoreBroker::registerConsumerConnection(const ConnectionInfo& ci) {
    logger::trace("Broker::registerConsumerConnection({}", str(ci));
    return this->process_->registerConsumerConnection(ci);
}

Value CoreBroker::registerProviderConnection(const Value& ci) {
    logger::trace("Broker::registerProviderConnection({}", str(ci));
    return this->process_->registerProviderConnection(ci);
}

Value CoreBroker::removeConsumerConnection(const ConnectionInfo& ci) {
    logger::trace("Broker::removeConsumerConnection({}", str(ci));
    return this->process_->deleteConsumerConnection(ci);
}

Value CoreBroker::removeProviderConnection(const ConnectionInfo& ci) {
    logger::trace("Broker::removeProviderConnection({}", str(ci));
    return this->process_->deleteProviderConnection(ci);
}

Value CoreBroker::putToArgument(const Value& opInfo, const std::string& argName, const Value& value) {
    logger::trace("Broker::putToArgument()");
    return this->process_->putToArgument(opInfo, argName, value);
}

Value CoreBroker::putToArgumentViaConnection(const Value& conInfo, const Value& value) {
    logger::trace("Broker::putToArgumentViaConnection()");
    return this->process_->putToArgumentViaConnection(conInfo, value);
}

Value CoreBroker::createResource(const std::string& path, const Value& value) {
    return process_->createResource(path, value);
}

Value CoreBroker::readResource(const std::string& path) const {
    return process_->readResource(path);//
}

Value CoreBroker::updateResource(const std::string& path, const Value& value) {
    return process_->updateResource(path, value);
}

Value CoreBroker::deleteResource(const std::string& path) {
    return process_->deleteResource(path);
}