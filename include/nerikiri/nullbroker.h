#pragma once



#include "nerikiri/brokerapi.h"


namespace nerikiri {


    class NullBroker : public BrokerAPI {
    private:

    public:
        NullBroker() : BrokerAPI() {}

        virtual ~NullBroker() {}

        virtual Value getBrokerInfo() const override {
            return Value::error("BrokerAPI::getBrokerInfo() failed. Broker is null.");
        }

        virtual Value getProcessInfo() const override {
            return Value::error("BrokerAPI::getProcessInfo() failed. Broker is null.");
        }

        virtual Value getOperationInfos() const override {
            return Value::error("BrokerAPI::getOperationInfos() failed. Broker is null.");
        }

        virtual Value getOperationInfo(const std::string& name) const override {
            return Value::error("BrokerAPI::getOperationInfo() failed. Broker is null.");
        }

        virtual Value getAllOperationInfos() const override {
            return Value::error("BrokerAPI::getAllOperationInfo() failed. Broker is null.");
        }

        virtual Value getContainerInfos() const override  {
            return Value::error("BrokerAPI::getContainerInfos() failed. Broker is null.");
        }

        virtual Value getContainerInfo(const std::string& fullName) const override {
            return Value::error("BrokerAPI::getContainerInfo() failed. Broker is null.");
        }

        virtual Value getContainerOperationInfos(const std::string& fullName) const override {
            return Value::error("BrokerAPI::getContainerOperationInfos() failed. Broker is null.");
        }

        virtual Value getContainerOperationInfo(const std::string& fullName) const override {
            return Value::error("BrokerAPI::getContainerOperationInfo() failed. Broker is null.");
        }

        virtual Value callContainerOperation(const std::string& fullName, const Value& arg) override {
            return Value::error("BrokerAPI::callContainerOperation() failed. Broker is null.");
        }

        virtual Value invokeContainerOperation(const std::string& fullName) const override {
            return Value::error("BrokerAPI::invokeContainerOperation() failed. Broker is null.");
        }

        virtual Value callOperation(const std::string& fullName, const Value& value) override {
            return Value::error("BrokerAPI::callOperation() failed. Broker is null.");
        }

        virtual Value invokeOperation(const std::string& fullName) const override {
            return Value::error("BrokerAPI::invokeOperation() failed. Broker is null.");
        }

        virtual Value executeOperation(const std::string& fullName) override {
            return Value::error("BrokerAPI::executeOperation() failed. Broker is null.");
        }

        virtual Value getConnectionInfos() const override {
            return Value::error("BrokerAPI::getConnectionInfos() failed. Broker is null.");
        }

        virtual Value registerConsumerConnection(const Value& ci) override {
            return Value::error("BrokerAPI::registerConsumerConnection() failed. Broker is null.");
        }

        virtual Value registerProviderConnection(const Value& ci) override {
            return Value::error("BrokerAPI::registerProviderConnection() failed. Broker is null.");
        }

        virtual Value removeProviderConnection(const std::string& operationFullName, const std::string& conName) override {
            return Value::error("BrokerAPI::removeProviderConnection() failed. Broker is null.");
        }

        virtual Value removeConsumerConnection(const std::string& operationFullName, const std::string& targetArgName, const std::string& conName) override {
            return Value::error("BrokerAPI::removeConsumerConnection() failed. Broker is null.");
        }

        virtual Value putToArgument(const std::string& fullName, const std::string& argName, const Value& value) override {
            return Value::error("BrokerAPI::putToArgument() failed. Broker is null.");
        }

        virtual Value putToArgumentViaConnection(const std::string& fullName, const std::string& targetArgName, const std::string& conName, const Value& value) override {
            return Value::error("BrokerAPI::putToArgumentViaConnection() failed. Broker is null.");
        }


        virtual Value getOperationFactoryInfos() const override {
            return Value::error("BrokerAPI::getOperationFactoryInfos() failed. Broker is null.");
        }
        virtual Value getContainerFactoryInfos() const override {
            return Value::error("BrokerAPI::getContainerFactoryInfos() failed. Broker is null.");
        }

        virtual Value createOperation(const Value& value) override {
            return Value::error("BrokerAPI::createOperation({}) failed. Broker is null.");
        }

        virtual Value createContainer(const Value& value) override {
            return Value::error("BrokerAPI::createContainer({}) failed. Broker is null.");
        }

        virtual Value createContainerOperation(const std::string& fullName, const Value& value) override {
            return Value::error("BrokerAPI::createContainerOperation({}) failed. Broker is null.");
        }

        virtual Value deleteOperation(const std::string& fullName) override {
            return Value::error("BrokerAPI::createContainerOperation({}) failed. Broker is null.");
        }

        virtual Value deleteContainer(const std::string& fullName) override {
            return Value::error("BrokerAPI::createContainerOperation({}) failed. Broker is null.");
        }

        virtual Value deleteContainerOperation(const std::string& fullName) override {
            return Value::error("BrokerAPI::createContainerOperation({}) failed. Broker is null.");
        }

        virtual Value getExecutionContextFactoryInfos() const override {
             return Value::error("BrokerAPI::getExecutionContextFactoryInfos({}) failed. Broker is null.");
        }

        virtual Value createExecutionContext(const Value& value) override {
            return Value::error("BrokerAPI::createContainerOperation({}) failed. Broker is null.");
        }

        virtual Value deleteExecutionContext(const std::string& fullName) override {
            return Value::error("BrokerAPI::deleteExecutionContext({}) failed. Broker is null.");
        }


        virtual Value getOperationConnectionInfos(const std::string& fullName) const override {
            return Value::error("BrokerAPI::getOperationConnectionInfos({}) failed. Broker is null.");
        }

        virtual Value getContainerOperationConnectionInfos(const std::string& fullName) const override {
            return Value::error("BrokerAPI::getContainerOperationConnectionInfos({}) failed. Broker is null.");
        }

        virtual Value getAllOperationConnectionInfos(const std::string& fullName) const override {
            return Value::error("BrokerAPI::getAllOperationConnectionInfos({}) failed. Broker is null.");

        }

        virtual Value executeContainerOperation(const std::string& fullName)  override {
            return Value::error("BrokerAPI::getOperationConnectionInfos({}) failed. Broker is null.");
        }
        
        virtual Value getAllOperationInfo(const std::string& fullName) const  override {
            return Value::error("BrokerAPI::getAllOperationInfo({}) failed. Broker is null.");
        }

        virtual Value invokeAllOperation(const std::string& fullName) const  override {
            return Value::error("BrokerAPI::invokeAllOperation({}) failed. Broker is null.");
        }

        virtual Value callAllOperation(const std::string& fullName, const Value& arg) override {
            return Value::error("BrokerAPI::callAllOperation({}) failed. Broker is null.");
        }

        virtual Value executeAllOperation(const std::string& fullName) override {
            return Value::error("BrokerAPI::createContainerOperation({}) failed. Broker is null.");
        }

        virtual Value getExecutionContextInfos() const override {
            return Value::error("BrokerAPI::getExecutionContextInfos({}) failed. Broker is null.");
        }

        virtual Value getExecutionContextInfo(const std::string& fullName) const override {
            return Value::error("BrokerAPI::getExecutionContextInfo({}) failed. Broker is null.");
        }

        virtual Value getExecutionContextState(const std::string& fullName) const override {
            return Value::error("BrokerAPI::getExecutionContextState({}) failed. Broker is null.");
        }

        virtual Value setExecutionContextState(const std::string& fullName, const std::string& state)  override {
            return Value::error("BrokerAPI::setExecutionContextState({}) failed. Broker is null.");
        }


        virtual Value getExecutionContextBoundOperationInfos(const std::string& fullName) const override {
            return Value::error("BrokerAPI::getExecutionContextBoundOperationInfos({}) failed. Broker is null.");
        }

        virtual Value getExecutionContextBoundAllOperationInfos(const std::string& fullName) const override {
            return Value::error("BrokerAPI::getExecutionContextBoundAllOperationInfos({}) failed. Broker is null.");
        }

        virtual Value bindOperationToExecutionContext(const std::string& ecFullName, const std::string& opFullName, const Value& opInfo)  override {
            return Value::error("BrokerAPI::bindOperationToExecutionContext({}) failed. Broker is null.");
        }

        virtual Value unbindOperationFromExecutionContext(const std::string& ecFullName, const std::string& opFullName)  override {
            return Value::error("BrokerAPI::unbindOperationFromExecutionContext({}) failed. Broker is null.");
        }


        virtual Value getBrokerInfos() const  override {
            return Value::error("BrokerAPI::getBrokerInfos({}) failed. Broker is null.");
        }

        virtual Value getCallbacks() const  override {
            return Value::error("BrokerAPI::getCallbacks({}) failed. Broker is null.");
        }

        virtual Value getTopicInfos() const override {
            return Value::error("BrokerAPI::getTopicInfos({}) failed. Broker is null.");
        }

        virtual Value invokeTopic(const std::string& fullName) const override {
            return Value::error("BrokerAPI::invokeTopic({}) failed. Broker is null.");
        }


        virtual Value getTopicConnectionInfos(const std::string& fullName) const override {
            return Value::error("BrokerAPI::getTopicConnectionInfos({}) failed. Broker is null.");
        }

        virtual Value createResource(const std::string& path, const Value& value) override {
            return Value::error("BrokerAPI::createResource() failed. Broker is null.");
        }

        virtual Value readResource(const std::string& path) const override {
            return Value::error("BrokerAPI::readResource() failed. Broker is null.");
        }

        virtual Value updateResource(const std::string& path, const Value& value) override {
            return Value::error("BrokerAPI::updateResource() failed. Broker is null.");
        }

        virtual Value deleteResource(const std::string& path) override {
            return Value::error("BrokerAPI::deleteResource() failed. Broker is null.");
        }
    };
}