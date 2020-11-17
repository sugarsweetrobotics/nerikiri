#include <utility>

#include "nerikiri/process.h"
#include "nerikiri/corebroker.h"
#include "nerikiri/operation.h"
#include "nerikiri/connection.h"
#include "nerikiri/object_mapper.h"
#include "nerikiri/objectfactory.h"
// #include "nerikiri/connection_builder.h"

using namespace nerikiri;

Value CoreBroker::getProcessInfo() const{ return process_->info(); }

Value CoreBroker::getProcessFullInfo() const { return process_->fullInfo(); }

Value CoreFactoryBroker::createObject(const std::string& className, const Value& info/*={}*/) {
    logger::trace("CoreBroker::createObject({}, {})", className, info);
    if (className == "operation") {
        return ObjectFactory::createOperation(*process_->store(), info);
    } else if (className == "container") {
        return ObjectFactory::createContainer(*process_->store(), info);
    } else if (className == "containerOperation") {
        return ObjectFactory::createContainerOperation(*process_->store(), info);
    } else if (className == "ec") {
        return ObjectFactory::createExecutionContext(*process_->store(), info);
    } else if (className == "fsm") {
        return ObjectFactory::createFSM(*process_->store(), info);
    }

    return Value::error(logger::error("CoreBroker::createObject({}, {}) failed. Class name is invalid.", className, info));
}

Value CoreFactoryBroker::deleteObject(const std::string& className, const std::string& fullName) {
    logger::trace("CoreBroker::deleteObject({})", fullName);
    if (className == "operation") {
        return ObjectFactory::deleteOperation(*process_->store(), fullName);
    } else if (className == "container") {
        return ObjectFactory::deleteContainer(*process_->store(), fullName);
    } else if (className == "containerOperation") {
        return ObjectFactory::deleteContainerOperation(*process_->store(), fullName);
    } else if (className == "ec") {
        return ObjectFactory::deleteExecutionContext(*process_->store(), fullName);
    } else if (className == "fsm") {
        return ObjectFactory::deleteFSM(*process_->store(), fullName);
    }

    return Value::error(logger::error("CoreBroker::deleteObject({}, {}) failed. Class name is invalid.", className, fullName));
}

Value CoreStoreBroker::getClassObjectInfos(const std::string& className) const {
    logger::trace("CoreBroker::getClassObjectInfos({})", className);
    if (className == "operation") {
        return nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(process_->store()->operations(), [](auto op) { return op->info(); });
    } else if (className == "operationFactory") {
        return nerikiri::functional::map<Value, std::shared_ptr<OperationFactoryAPI>>(process_->store()->operationFactories(), [](auto op) { return op->info(); });
    } else if (className == "container") {
        return nerikiri::functional::map<Value, std::shared_ptr<ContainerAPI>>(process_->store()->containers(), [](auto o) { return o->info(); });
    } else if (className == "containerFactory") {
        return nerikiri::functional::map<Value, std::shared_ptr<ContainerFactoryAPI>>(process_->store()->containerFactories(), [](auto o) { return o->info(); });
    } else if (className == "ec") {
        return nerikiri::functional::map<Value, std::shared_ptr<ExecutionContextAPI>>(process_->store()->executionContexts(), [](auto o) { return o->info(); });
    } else if (className == "topic") {
        return nerikiri::functional::map<Value, std::shared_ptr<TopicBase>>(process_->store()->topics(), [](auto o) { return o->info(); });
    } else if (className == "ecFactory") {
        return nerikiri::functional::map<Value, std::shared_ptr<ExecutionContextFactoryAPI>>(process_->store()->executionContextFactories(), [](auto o) { return o->info(); });
    } else if (className == "fsm") {
        return nerikiri::functional::map<Value, std::shared_ptr<FSMAPI>>(process_->store()->fsms(), [](auto o) { return o->info(); });
    } else if (className == "fsmFactory") {
        return nerikiri::functional::map<Value, std::shared_ptr<FSMFactoryAPI>>(process_->store()->fsmFactories(), [](auto o) { return o->info(); });
    } else if (className == "broker") {
        return nerikiri::functional::map<Value, std::shared_ptr<BrokerAPI>>(process_->store()->brokers(), [](auto o) { return o->info(); });
    } else if (className == "brokerFactory") {
        return nerikiri::functional::map<Value, std::shared_ptr<BrokerFactoryAPI>>(process_->store()->brokerFactories(), [](auto o) { return o->info(); });
    }
    return Value::error(logger::error("CoreBroker::getClassObjectInfos({}) failed. Class name is invalid.", className));
}

Value CoreStoreBroker::getChildrenClassObjectInfos(const std::string& parentName, const std::string& className) const {

}

Value CoreStoreBroker::getObjectInfo(const std::string& className, const std::string& fullName) const {
    logger::trace("CoreBroker::getObjectInfo({}, {})", className, fullName);
    if (className == "operation") {
        return process_->store()->operation(fullName)->fullInfo();
    } else if (className == "container") {
        return process_->store()->container(fullName)->info();
    }

    return Value::error(logger::error("CoreBroker::getObjectInfo({}, {}) failed. Class name is invalid.", className, fullName));
}

//std::shared_ptr<BrokerAPI>  Broker::null = std::shared_ptr<BrokerAPI>(nullptr);

/*

Value CoreBroker::getOperationInfos() const {
    return nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(process_->store()->operations(), [](const auto& op) -> Value { return op->info(); });
}

Value CoreBroker::getAllOperationInfos() const {
    //return process_->store()->getAllOperationInfos();
    return getOperationInfos();
}

Value CoreBroker::getContainerInfos() const {
    //return process_->store()->getContainerInfos();
    return nerikiri::functional::map<Value, std::shared_ptr<ContainerAPI>>(process_->store()->containers(), [](const auto& c) { return c->info(); });
}

Value CoreBroker::getContainerInfo(const std::string& fullName) const {
    return process_->store()->container(fullName)->info();
    //return process_->store()->getContainer(fullName)->info();
}

Value CoreBroker::getContainerOperationInfos(const std::string& fullName) const {
    return nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(process_->store()->container(fullName)->operations(), [](auto op) { return op->info(); });
    //return process_->store()->getContainer(fullName)->getOperationInfos();
}

Value CoreBroker::getContainerOperationInfo(const std::string& fullName) const {
    return process_->store()->operation(fullName)->info();
    //auto [containerName, operationName] = splitContainerAndOperationName(fullName);
    //return process_->store()->getContainer(containerName)->getOperation(operationName)->info();
}

Value CoreBroker::getOperationInfo(const std::string& fullName) const {
    //return process_->store()->getAllOperation(fullName)->info();
    return process_->store()->container(fullName)->info();
}

Value CoreBroker::callContainerOperation(const std::string& fullName, const Value& arg) {
    return process_->store()->operation(fullName)->call(arg);
    //auto [containerName, operationName] = splitContainerAndOperationName(fullName);
    //return process_->store()->getContainer(containerName)->getOperation(operationName)->call(std::move(arg));
    //return process_->store()->container(containerName)->operation(operationName)->call(std::move(arg));
}

Value CoreBroker::invokeContainerOperation(const std::string& fullName) const {
    return process_->store()->operation(fullName)->invoke();
    //auto [containerName, operationName] = splitContainerAndOperationName(fullName);
    //return process_->store()->getContainer(containerName)->getOperation(operationName)->invoke();
}

Value CoreBroker::callOperation(const std::string& fullName, const Value& value) {
    return process_->store()->operation(fullName)->call(value);
}

Value CoreBroker::invokeOperation(const std::string& fullName) const {
    //return process_->store()->getAllOperation(fullName)->invoke();
    return process_->store()->operation(fullName)->invoke();
}

Value CoreBroker::executeOperation(const std::string& fullName) {
    //logger::trace("CoreBroker::executeOperation({})", fullName);
    //return process_->store()->getAllOperation(fullName)->execute();
    return process_->store()->operation(fullName)->execute();
}


Value CoreBroker::callAllOperation(const std::string& fullName, const Value& value) {
    //return process_->store()->getAllOperation(fullName)->call(std::move(value));
    return process_->store()->operation(fullName)->call(value);
}

Value CoreBroker::invokeAllOperation(const std::string& fullName) const {
    logger::trace("CoreBroker::invokeAllOperation({})", fullName);
    //return process_->store()->getAllOperation(fullName)->invoke();
    return process_->store()->operation(fullName)->invoke();
}

Value CoreBroker::executeAllOperation(const std::string& fullName) {
    logger::trace("CoreBroker::executeAllOperation({})", fullName);
    //return process_->store()->getAllOperation(fullName)->execute();
    return process_->store()->operation(fullName)->execute();
}


Value CoreBroker::getConnectionInfos() const {
    std::vector<std::shared_ptr<ConnectionAPI>> connections;
    for(auto op : process_->store()->operations()) {
        auto cons = op->outlet()->connections();
        connections.insert(connections.end(), cons.begin(), cons.end());
    }
    return nerikiri::functional::map<Value, std::shared_ptr<ConnectionAPI>>(connections, [](auto c) { return c->info(); });
}

Value CoreBroker::createConnection(const Value& connectionInfo, const Value& brokerInfo) {
    logger::trace("CoreBroker::registerConsumerConnection({}");
    return ConnectionBuilder::createConnection(process_->store(), connectionInfo);
}


Value CoreBroker::registerConsumerConnection(const Value& ci) {
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
}

Value CoreBroker::createContainer(const Value& value) {
    logger::trace("CoreBroker::createContainer({})", value);
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
    logger::trace("CoreBroker::setExecutionContextState({}, {})", fullName, state);
    return process_->store()->getExecutionContext(fullName)->setState(state);
}

Value CoreBroker::executeContainerOperation(const std::string& fullName) {
    //return process_->store()->getContainerOperation(fullName)->execute();
    return process_->store()->operation(fullName)->execute();
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
    //return process_->store()->getAllOperation(fullName)->info();
    return nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(
        process_->store()->operations(), [](auto op) { return op->info(); });
    
}

Value CoreBroker::getExecutionContextInfo(const std::string& fullName) const {
    return process_->store()->getExecutionContext(fullName)->info();
}

Value CoreBroker::getTopicConnectionInfos(const std::string& fullName) const {
    return process_->store()->getTopic(fullName)->getConnectionInfos();
}

Value CoreBroker::getExecutionContextState(const std::string& fullName) const {
    return process_->store()->getExecutionContext(fullName)->getObjectState();
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
    logger::trace("CoreBroker::getFSMInfos() called");
    return nerikiri::functional::map<Value, std::shared_ptr<FSMAPI>>(process_->store()->fsms(), [](auto f) {
        return f->info();
    });
}

Value CoreBroker::getFSMInfo(const std::string& fullName) const {
    //return process_->store()->getFSM(fullName)->info();
    logger::trace("CoreBroker::getFSMInfo({}) called", fullName);
    return process_->store()->fsm(fullName)->info();
}

Value CoreBroker::setFSMState(const std::string& fullName, const std::string& state) {
    logger::trace("CoreBroker::setFSMState({}, {})", fullName, state);
    ///return process_->store()->getFSM(fullName)->setFSMState(state);
    return process_->store()->fsm(fullName)->setFSMState(state);
}

Value CoreBroker::getFSMState(const std::string& fullName) const {
    //return process_->store()->getFSM(fullName)->getFSMState();
    logger::trace("CoreBroker::getFSMState({})", fullName);
    return process_->store()->fsm(fullName)->currentFsmState()->info();
}

Value CoreBroker::getOperationsBoundToFSMState(const std::string& fsmFullName, const std::string& stateName) {
    return nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(process_->store()->fsm(fsmFullName)->fsmState(stateName)->boundOperations(), [](auto op) {
        return op->info();
    });
    //auto ops = process_->store()->getFSM(fsmFullName)->getBoundOperations(state);
    //Value v = Value::list();
    //for(auto op : ops) {
    //    v.push_back(op->info());
    //}
    //return v;
}

Value CoreBroker::getECsBoundToFSMState(const std::string& fsmFullName, const std::string& state) {
    auto ecAndStates = process_->store()->getFSM(fsmFullName)->getBoundECs(state);
    Value v = Value::list();
    for(auto ecAndState : ecAndStates) {
        if (ecAndState.first == state) {
            v.push_back(ecAndState.second->info());
        }
    }
    return v;
}


Value CoreBroker::getFSMsBoundToFSMState(const std::string& fsmFullName, const std::string& state) {
    
}

Value CoreBroker::bindOperationToFSMState(const std::string& fsmFullName, const std::string& state, const Value& operation) {
    
}

Value CoreBroker::bindECStateToFSMState(const std::string& fsmFullName, const std::string& state, const Value& ecState) {
    
}

Value CoreBroker::bindFSMStateToFSMState(const std::string& fsmFullName, const std::string& state, const Value& fsmState) {
    
}
*/
