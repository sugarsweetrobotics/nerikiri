#include <utility>

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

Value CoreBroker::getAllOperationInfos() const {
    return process_->store()->getAllOperationInfos();
}

Value CoreBroker::getContainerInfos() const {
    return process_->store()->getContainerInfos();
}

Value CoreBroker::getContainerInfo(const std::string& fullName) const {
    return process_->store()->getContainer(fullName)->info();
}

Value CoreBroker::getContainerOperationInfos(const std::string& fullName) const {
    return process_->store()->getContainer(fullName)->getOperationInfos();
}

Value CoreBroker::getContainerOperationInfo(const std::string& fullName) const {
    auto [containerName, operationName] = splitContainerAndOperationName(fullName);
    return process_->store()->getContainer(containerName)->getOperation(operationName)->info();
}

Value CoreBroker::getOperationInfo(const std::string& fullName) const {
    return process_->store()->getOperation(fullName)->info();
}

Value CoreBroker::callContainerOperation(const std::string& fullName, const Value& arg) {
    auto [containerName, operationName] = splitContainerAndOperationName(fullName);
    return process_->store()->getContainer(containerName)->getOperation(operationName)->call(std::move(arg));
}

Value CoreBroker::invokeContainerOperation(const std::string& fullName) const {
    auto [containerName, operationName] = splitContainerAndOperationName(fullName);
    return process_->store()->getContainer(containerName)->getOperation(operationName)->invoke();
}

Value CoreBroker::callOperation(const std::string& fullName, const Value& value) {
    return process_->store()->getAllOperation(fullName)->call(std::move(value));
}

Value CoreBroker::invokeOperation(const std::string& fullName) const {
    return process_->store()->getAllOperation(fullName)->invoke();
}

Value CoreBroker::executeOperation(const std::string& fullName) {
    logger::trace("CoreBroker::executeOperation({})", fullName);
    return process_->store()->getAllOperation(fullName)->execute();
}


Value CoreBroker::callAllOperation(const std::string& fullName, const Value& value) {
    return process_->store()->getAllOperation(fullName)->call(std::move(value));
}

Value CoreBroker::invokeAllOperation(const std::string& fullName) const {
    logger::trace("CoreBroker::invokeAllOperation({})", fullName);
    return process_->store()->getAllOperation(fullName)->invoke();
}

Value CoreBroker::executeAllOperation(const std::string& fullName) {
    logger::trace("CoreBroker::executeAllOperation({})", fullName);
    return process_->store()->getAllOperation(fullName)->execute();
}


Value CoreBroker::getConnectionInfos() const {
    return process_->store()->getConnectionInfos();
}

Value CoreBroker::createConnection(const Value& connectionInfo, const Value& brokerInfo) {
    logger::trace("CoreBroker::registerConsumerConnection({}");
    return ConnectionBuilder::registerProviderConnection(process_->store(), connectionInfo);
}


Value CoreBroker::registerConsumerConnection(const ConnectionInfo& ci) {
    logger::trace("CoreBroker::registerConsumerConnection({}", str(ci));
    return ConnectionBuilder::registerConsumerConnection(process_->store(), ci);
}

Value CoreBroker::registerProviderConnection(const Value& ci) {
    logger::trace("CoreBroker::registerProviderConnection({}", str(ci));
    return ConnectionBuilder::registerProviderConnection(process_->store(), ci);
}

Value CoreBroker::removeConsumerConnection(const std::string& operationFullName, const std::string& targetArgName, const std::string& conName) {
    logger::trace("CoreBroker::removeConsumerConnection({}", conName);
    return ConnectionBuilder::deleteConsumerConnection(process_->store(), operationFullName, targetArgName, conName);
}

Value CoreBroker::removeProviderConnection(const std::string& operationFullName, const std::string& conName) {
    logger::trace("CoreBroker::removeProviderConnection({}", conName);
    return ConnectionBuilder::deleteProviderConnection(process_->store(), operationFullName, conName);
}

Value CoreBroker::putToArgument(const std::string& fullName, const std::string& argName, const Value& value) {
    logger::trace("CoreBroker::putToArgument()");
    return process_->store()->getOperationOrTopic(fullName)->putToArgument(argName, value);
}

Value CoreBroker::putToArgumentViaConnection(const std::string& fullName, const std::string& argName, const std::string& conName, const Value& value) {
    logger::trace("CoreBroker::putToArgumentViaConnection({})", conName);
    return process_->store()->getOperationOrTopic(fullName)->putToArgumentViaConnection(
        argName, conName, value);
}

Value CoreBroker::getOperationFactoryInfos() const {
    logger::trace("CoreBroker::getOperationFactoryInfos()");
    return process_->store()->getOperationFactoryInfos();
}

Value CoreBroker::getContainerFactoryInfos() const {
    logger::trace("CoreBroker::getContainerFactoryInfos()");
    return process_->store()->getContainerFactoryInfos();
}


Value CoreBroker::createOperation(const Value& value) {
    logger::trace("CoreBroker::createOperation({})", value);
    return ObjectFactory::createOperation(*process_->store(), value);
}

Value CoreBroker::createContainer(const Value& value) {
    logger::trace("CoreBroker::createContainer({})", value);
    return ObjectFactory::createContainer(*process_->store(), value);
}


Value CoreBroker::createContainerOperation(const std::string& fullName, const Value& value) {
    logger::trace("Broker::createContainerOperation({})", value);
    return process_->store()->getContainer(fullName)->createContainerOperation(value);
}

Value CoreBroker::deleteOperation(const std::string& fullName) {
    logger::trace("Broker::deleteOperation({})", fullName);
    return ObjectFactory::deleteOperation(*process_->store(), fullName);
}

Value CoreBroker::deleteContainer(const std::string& fullName) {
    logger::trace("CoreBroker::deleteContainer({})", fullName);
    return ObjectFactory::deleteContainer(*process_->store(), fullName);
    
}

Value CoreBroker::deleteContainerOperation(const std::string& fullName) { 
    logger::trace("CoreBroker::deleteContainerOperation({})", fullName);
    auto [containerName, operationName] = splitContainerAndOperationName(fullName);
    return process_->store()->getContainer(containerName)->deleteContainerOperation(operationName);
}

Value CoreBroker::createExecutionContext(const Value& value) {
    logger::trace("CoreBroker::createExecutionContext({})", value);
    return ObjectFactory::createExecutionContext(*process_->store(), value);
    
}
Value CoreBroker::deleteExecutionContext(const std::string& fullName) {
    logger::trace("CoreBroker::deleteExecutionContext({})", fullName);
    return ObjectFactory::deleteExecutionContext(*process_->store(), fullName);
}

Value CoreBroker::createResource(const std::string& path, const Value& value) {
    //return process_->createResource(path, value);
    return ObjectMapper::createResource(this, path, std::move(value));
}

Value CoreBroker::readResource(const std::string& path) const {
    //return process_->readResource(path);//
    return ObjectMapper::readResource(this, path);
}

Value CoreBroker::updateResource(const std::string& path, const Value& value) {
    //return process_->updateResource(path, value);
    return ObjectMapper::updateResource(this, path, std::move(value));
}

Value CoreBroker::deleteResource(const std::string& path) {
    //return process_->deleteResource(path);
    return ObjectMapper::deleteResource(this, path);
}


Value CoreBroker::setExecutionContextState(const std::string& fullName, const std::string& state) {
    return process_->store()->getExecutionContext(fullName)->setState(state);
}

Value CoreBroker::executeContainerOperation(const std::string& fullName) {
    return process_->store()->getContainerOperation(fullName)->execute();
}

Value CoreBroker::bindOperationToExecutionContext(const std::string& ecFullName, const std::string& opFullName, const Value& brokerInfo) {
    //auto broker = process_->store()->getBrokerFactory(brokerInfo)->createProxy(brokerInfo);
    //return process_->store()->getExecutionContext(ecFullName)->bind(opFullName, broker);
    return process_->store()->getExecutionContext(ecFullName)->bind(process_->store()->getAllOperation(opFullName));
}

Value CoreBroker::unbindOperationFromExecutionContext(const std::string& ecFullName, const std::string& opFullName) {
    return process_->store()->getExecutionContext(ecFullName)->unbind(opFullName);
}

Value CoreBroker::getBrokerInfos() const {
    return process_->store()->getBrokerInfos();
}

Value CoreBroker::getCallbacks() const {
    return process_->store()->getCallbacks();
}

Value CoreBroker::getTopicInfos() const {
    return process_->store()->getTopicInfos();
}

Value CoreBroker::invokeTopic(const std::string& fullName) const {
    return process_->store()->getTopic(fullName)->invoke();
}

Value CoreBroker::getExecutionContextInfos() const {
    return process_->store()->getExecutionContextInfos();
}

Value CoreBroker::getAllOperationInfo(const std::string& fullName) const {
    return process_->store()->getAllOperation(fullName)->info();
}

Value CoreBroker::getExecutionContextInfo(const std::string& fullName) const {
    return process_->store()->getExecutionContext(fullName)->info();
}

Value CoreBroker::getTopicConnectionInfos(const std::string& fullName) const {
    return process_->store()->getTopic(fullName)->getConnectionInfos();
}

Value CoreBroker::getExecutionContextState(const std::string& fullName) const {
    return process_->store()->getExecutionContext(fullName)->getState();
}

Value CoreBroker::getOperationConnectionInfos(const std::string& fullName) const {
    return process_->store()->getOperation(fullName)->getConnectionInfos();
}

Value CoreBroker::getAllOperationConnectionInfos(const std::string& fullName) const {
    return process_->store()->getAllOperation(fullName)->getConnectionInfos();
}

Value CoreBroker::getExecutionContextFactoryInfos() const {
    return process_->store()->getExecutionContextFactoryInfos();
}

Value CoreBroker::getContainerOperationConnectionInfos(const std::string& fullName) const {
    return process_->store()->getContainerOperation(fullName)->getConnectionInfos();
}

Value CoreBroker::getExecutionContextBoundOperationInfos(const std::string& fullName) const {
    return process_->store()->getExecutionContext(fullName)->getBoundOperationInfos();
}

Value CoreBroker::getExecutionContextBoundAllOperationInfos(const std::string& fullName) const {
    return process_->store()->getExecutionContext(fullName)->getBoundOperationInfos();
}


Value CoreBroker::getFSMInfos() const {
    return process_->store()->getFSMInfos();
}

Value CoreBroker::setFSMState(const std::string& fullName, const std::string& state) {
    return process_->store()->getFSM(fullName)->setFSMState(state);
}

Value CoreBroker::getFSMState(const std::string& fullName) const {
    return process_->store()->getFSM(fullName)->getFSMState();
}

Value CoreBroker::getOperationsBoundToFSMState(const std::string& fsmFullName, const std::string& state) {
    
}

Value CoreBroker::getECsBoundToFSMState(const std::string& fsmFullName, const std::string& state) {
    
}

Value CoreBroker::getFSMsBoundToFSMState(const std::string& fsmFullName, const std::string& state) {
    
}

Value CoreBroker::bindOperationToFSMState(const std::string& fsmFullName, const std::string& state, const Value& operation) {
    
}

Value CoreBroker::bindECStateToFSMState(const std::string& fsmFullName, const std::string& state, const Value& ecState) {
    
}

Value CoreBroker::bindFSMStateToFSMState(const std::string& fsmFullName, const std::string& state, const Value& fsmState) {
    
}
