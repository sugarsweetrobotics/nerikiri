

#include "nerikiri/process.h"
#include "nerikiri/corebroker.h"
#include "nerikiri/operation.h"
#include "nerikiri/connection.h"
#include "nerikiri/objectmapper.h"
#include "nerikiri/objectfactory.h"
#include "nerikiri/connectionbuilder.h"

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

Value CoreBroker::callContainerOperation(const Value& cinfo, const Value& oinfo, Value&& arg) {
    return process_->store()->getContainer(cinfo)->getOperation(oinfo)->call(std::move(arg));
}

Value CoreBroker::invokeContainerOperation(const Value& cinfo, const Value& oinfo) const {
    return process_->store()->getContainer(cinfo)->getOperation(oinfo)->invoke();
}

Value CoreBroker::callOperation(const Value& info, Value&& value) {
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
    return ConnectionBuilder::registerConsumerConnection(process_->store(), ci);
}

Value CoreBroker::registerProviderConnection(const Value& ci) {
    logger::trace("Broker::registerProviderConnection({}", str(ci));
    return ConnectionBuilder::registerProviderConnection(process_->store(), ci);
}

Value CoreBroker::removeConsumerConnection(const ConnectionInfo& ci) {
    logger::trace("Broker::removeConsumerConnection({}", str(ci));
    return ConnectionBuilder::deleteConsumerConnection(process_->store(), ci);
}

Value CoreBroker::removeProviderConnection(const ConnectionInfo& ci) {
    logger::trace("Broker::removeProviderConnection({}", str(ci));
    return ConnectionBuilder::deleteProviderConnection(process_->store(), ci);
}

Value CoreBroker::putToArgument(const Value& opInfo, const std::string& argName, const Value& value) {
    logger::trace("Broker::putToArgument()");
    //return this->process_->putToArgument(opInfo, argName, value);    
    return process_->store()->getOperationOrTopic(opInfo)->putToArgument(argName, value);
}

Value CoreBroker::putToArgumentViaConnection(const Value& conInfo, const Value& value) {
    logger::trace("Broker::putToArgumentViaConnection()");
    //return this->process_->putToArgumentViaConnection(conInfo, value);
    return process_->store()->getOperationOrTopic(conInfo.at("input").at("info"))->putToArgumentViaConnection(
        conInfo, value);
}

Value CoreBroker::getOperationFactoryInfos() const {
    logger::trace("Broker::getOperationFactoryInfos()");
    return process_->store()->getOperationFactoryInfos();
}

Value CoreBroker::getContainerFactoryInfos() const {
    logger::trace("Broker::getContainerFactoryInfos()");
    return process_->store()->getContainerFactoryInfos();
}


Value CoreBroker::createOperation(const Value& value) {
    logger::trace("Broker::createOperation({})", value);
    return ObjectFactory::createOperation(*process_->store(), value);
}

Value CoreBroker::createContainer(const Value& value) {
    logger::trace("Broker::createContainer({})", value);
    return ObjectFactory::createContainer(*process_->store(), value);
}


Value CoreBroker::createContainerOperation(const Value& containerInfo, const Value& value) {
    logger::trace("Broker::createContainerOperation({})", value);
    return process_->store()->getContainer(containerInfo)->createContainerOperation(value);
}

Value CoreBroker::deleteOperation(const Value& value) {
    logger::trace("Broker::deleteOperation({})", value);
    return ObjectFactory::deleteOperation(*process_->store(), value);
}

Value CoreBroker::deleteContainer(const Value& value) {
    logger::trace("Broker::deleteContainer({})", value);
    return ObjectFactory::deleteContainer(*process_->store(), value);
    
}

Value CoreBroker::deleteContainerOperation(const Value& value) { 
    logger::trace("Broker::deleteContainerOperation({})", value);
    
}
Value CoreBroker::createExecutionContext(const Value& value) {
    logger::trace("Broker::createExecutionContext({})", value);
    
}
Value CoreBroker::deleteExecutionContext(const Value& value) {
    logger::trace("Broker::deleteExecutionContext({})", value);
    
}

Value CoreBroker::createResource(const std::string& path, const Value& value) {
    //return process_->createResource(path, value);
    return ObjectMapper::createResource(process_->store(), path, value);
}

Value CoreBroker::readResource(const std::string& path) const {
    //return process_->readResource(path);//
    return ObjectMapper::readResource(process_->store(), path);
}

Value CoreBroker::updateResource(const std::string& path, const Value& value) {
    //return process_->updateResource(path, value);
    return ObjectMapper::updateResource(process_->store(), path, value);
}

Value CoreBroker::deleteResource(const std::string& path) {
    //return process_->deleteResource(path);
    return ObjectMapper::deleteResource(process_->store(), path);
}

