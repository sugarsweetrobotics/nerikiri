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

        virtual Value getOperationInfo(const Value& name) const override {
            return Value::error("BrokerAPI::getOperationInfo() failed. Broker is null.");
        }

        virtual Value getAllOperationInfos() const override {
            return Value::error("BrokerAPI::getAllOperationInfo() failed. Broker is null.");
        }

        virtual Value getContainerInfos() const override  {
            return Value::error("BrokerAPI::getContainerInfos() failed. Broker is null.");
        }

        virtual Value getContainerInfo(const Value& value) const override {
            return Value::error("BrokerAPI::getContainerInfo() failed. Broker is null.");
        }

        virtual Value getContainerOperationInfos(const Value& value) const override {
            return Value::error("BrokerAPI::getContainerOperationInfos() failed. Broker is null.");
        }

        virtual Value getContainerOperationInfo(const Value& cinfo, const Value& oinfo) const override {
            return Value::error("BrokerAPI::getContainerOperationInfo() failed. Broker is null.");
        }

        virtual Value callContainerOperation(const Value& cinfo, const Value& oinfo, Value&& arg) override {
            return Value::error("BrokerAPI::callContainerOperation() failed. Broker is null.");
        }

        virtual Value invokeContainerOperation(const Value& cinfo, const Value& oinfo) const override {
            return Value::error("BrokerAPI::invokeContainerOperation() failed. Broker is null.");
        }

        virtual Value callOperation(const Value& info, Value&& value) override {
            return Value::error("BrokerAPI::callOperation() failed. Broker is null.");
        }

        virtual Value invokeOperation(const Value& name) const override {
            return Value::error("BrokerAPI::invokeOperation() failed. Broker is null.");
        }

        virtual Value executeOperation(const Value& info) override {
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

        virtual Value removeProviderConnection(const Value& ci) override {
            return Value::error("BrokerAPI::removeProviderConnection() failed. Broker is null.");
        }

        virtual Value removeConsumerConnection(const Value& ci) override {
            return Value::error("BrokerAPI::removeConsumerConnection() failed. Broker is null.");
        }

        virtual Value putToArgument(const Value& opInfo, const std::string& argName, const Value& value) override {
            return Value::error("BrokerAPI::putToArgument() failed. Broker is null.");
        }

        virtual Value putToArgumentViaConnection(const Value& conInfo, const Value& value) override {
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

        virtual Value createContainerOperation(const Value& containerInfo, const Value& value) override {
            return Value::error("BrokerAPI::createContainerOperation({}) failed. Broker is null.");
        }

        virtual Value deleteOperation(const Value& value) override {
            return Value::error("BrokerAPI::createContainerOperation({}) failed. Broker is null.");
        }

        virtual Value deleteContainer(const Value& value) override {
            return Value::error("BrokerAPI::createContainerOperation({}) failed. Broker is null.");
        }

        virtual Value deleteContainerOperation(const Value& containerInfo, const Value& value) override {
            return Value::error("BrokerAPI::createContainerOperation({}) failed. Broker is null.");
        }

        virtual Value createExecutionContext(const Value& value) override {
            return Value::error("BrokerAPI::createContainerOperation({}) failed. Broker is null.");
        }

        virtual Value deleteExecutionContext(const Value& value) override {
            return Value::error("BrokerAPI::createContainerOperation({}) failed. Broker is null.");
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