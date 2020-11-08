#pragma once

#include <nerikiri/broker_proxy_api.h>
#include <nerikiri/naming.h>

namespace nerikiri {

  
  class AbstractBrokerProxy : public BrokerProxyAPI {
  public:
    AbstractBrokerProxy(const Value& v) : BrokerProxyAPI(v) {}
    virtual ~AbstractBrokerProxy() {}

  public:
  /*
    virtual Value getBrokerInfo() const override {
      return readResource("/broker/info/");
    }

    virtual Value getProcessInfo() const override {
      return readResource("/process/info/");
    }

    virtual Value getOperationInfos() const override {
        return readResource("/process/operations/");
    }

    virtual Value getAllOperationInfos() const override {
        return readResource("/process/all_operations/");
    }

    virtual Value getContainerInfos() const override {
        return readResource("/process/containers/");
    }

    virtual Value getConnectionInfos() const override {
        return readResource("/process/connections/");
    }

    virtual Value getContainerInfo(const std::string& fullName) const override {
        return readResource("/process/containers/" + fullName + "/");
    }

    virtual Value getContainerOperationInfos(const std::string& fullName) const override {
        return readResource("/process/containers/" + fullName + "/operations/");
    }

    virtual Value getContainerOperationInfo(const std::string& fullName) const override {
        auto [containerName, operationName] = splitContainerAndOperationName(fullName);
        return readResource("/process/containers/" + containerName + "/operations/" + operationName + "/info/");
    }

    virtual Value invokeContainerOperation(const std::string& fullName) const override {
        auto [containerName, operationName] = splitContainerAndOperationName(fullName);
        return readResource("/process/containers/" + containerName + "/operations/" + operationName + "/");
    }

    virtual Value executeOperation(const std::string& fullName) override {
        return updateResource("/process/operations/" + fullName + "/execution/", {});
    }

    virtual Value getOperationInfo(const std::string& fullName) const override {
        return readResource("/process/operations/" + fullName + "/info/");
    }

    virtual Value invokeOperation(const std::string& fullName) const override {
        return readResource("/process/operations/" + fullName + "/");
    }

    virtual Value registerConsumerConnection(const Value& ci) override {
      if (ci.isError()) return ci;    
      auto operation_name = ci.at("input").at("info").at("fullName").stringValue();
      auto argument_name  = ci.at("input").at("target").at("name").stringValue();
      return createResource("/process/operations/" + operation_name + "/input/arguments/" + argument_name + "/connections/", ci);
    }

    virtual Value createConnection(const Value& connectionInfo, const Value& brokerInfo) override {
        return createResource("/process/connections/", connectionInfo);
    }

    virtual Value removeConsumerConnection(const std::string& operationFullName, const std::string& targetArgName, const std::string& connectionName) override {
        return deleteResource("/process/operations/" + operationFullName + "/input/arguments/" + targetArgName + "/connections/" + connectionName + "/");
    }

    virtual Value registerProviderConnection(const Value& ci) override {
      if (ci.isError()) {
        logger::error("BrokerProxy::registerProviderConnection failed. ({})", str(ci));
        return ci;
      }
      if (ci.at("output").at("info").isError()) {
        logger::error("BrokerProxy::registerProviderConnection failed. ({})", str(ci));
        return ci.at("output").at("info");
      }
      if (ci.at("input").at("info").isError()) {
        logger::error("BrokerProxy::registerProviderConnection failed. ({})", str(ci));
        return ci.at("input").at("info");
      }
      auto operation_name = ci.at("input").at("info").at("fullName").stringValue();
      return createResource("/process/operations/" + operation_name + "/output/connections/", ci);
    }

    virtual Value removeProviderConnection(const std::string& operationFullName, const std::string& connectionName) override {
        return deleteResource("/process/operations/" + operationFullName + "/output/connections/" + connectionName + "/");
    }

    virtual Value callContainerOperation(const std::string& fullName, const Value& arg) override {
        auto [containerName, operationName] = splitContainerAndOperationName(fullName);
        return updateResource("/process/container/" + containerName + "/operation/" + operationName + "/", arg);
    }

    virtual Value callOperation(const std::string& fullName, const Value& value) override {
        return updateResource("/process/operation/" + fullName + "/", value);
    }

    virtual Value putToArgument(const std::string& fullName, const std::string& argName, const Value& value) override {
        return updateResource("/process/all_operations/" + fullName + "/input/arguments/" + argName + "/", value);
    }

    virtual Value putToArgumentViaConnection(const std::string& fullName, const std::string& targetArgName, const std::string& conName, const Value& value) override {
        return updateResource("/process/all_operations/" + fullName + "/input/arguments/" + targetArgName + "/connections/" + conName + "/", value);
    }

    virtual Value getOperationFactoryInfos() const override {
      return readResource("/process/operationFactories/");
      
    }

    virtual Value getContainerFactoryInfos() const override {
      return readResource("/process/containerFactories/");
    }

    virtual Value createOperation(const Value& value) override {
      return createResource("/process/operations/", value);
    }

    virtual Value createContainer(const Value& value) override {
      return createResource("/process/containers/", value);
    }

    virtual Value createContainerOperation(const std::string& fullName, const Value& value) override {
        return createResource("/process/containers/" +  fullName + "/operations/", value);
    }

    virtual Value deleteOperation(const std::string& fullName) override {
        return deleteResource("/process/operations/" + fullName + "/");
    }

    virtual Value getOperationConnectionInfos(const std::string& fullName) const override {
        return readResource("/process/operations/" + fullName + "/connections/");
    }

    virtual Value getContainerOperationConnectionInfos(const std::string& fullName) const override {
        auto [containerName, operationName] = splitContainerAndOperationName(fullName);
        return readResource("/process/containers/" + containerName + "/operations/" + operationName + "/connections/");
    }

    virtual Value executeContainerOperation(const std::string& fullName) override {
      auto [containerName, operationName] = splitContainerAndOperationName(fullName);
      return updateResource("/process/containers/" + containerName + "/operations/" + operationName + "/execution/", Value::object());
    }

    virtual Value getAllOperationInfo(const std::string& fullName) const override {
      return readResource("/process/all_operations/");
    }

    virtual Value invokeAllOperation(const std::string& fullName) const override {
      return readResource("/process/all_operations/" + fullName + "/");
    }
    
    virtual Value callAllOperation(const std::string& fullName, const Value& value) override {
      return updateResource("/process/all_operations/" + fullName + "/", value);
    }

    virtual Value executeAllOperation(const std::string& fullName) override {
      return updateResource("/process/all_operations/" + fullName + "/execution/", Value::object());
    }

    virtual Value getAllOperationConnectionInfos(const std::string& fullName) const override {
      return readResource("/process/all_operations/" + fullName + "/connections/");
    }

    virtual Value getExecutionContextInfos() const override {
      return readResource("/process/ecs/");
    }

    virtual Value getExecutionContextFactoryInfos() const override {
      return readResource("/process/ecfactories/");
    }

    virtual Value getExecutionContextInfo(const std::string& fullName) const override {
      return readResource("/process/ecs/" + fullName + "/");
    }

    virtual Value getExecutionContextState(const std::string& fullName) const override {
      return readResource("/process/ecs/" + fullName + "/state/");
    }

    virtual Value setExecutionContextState(const std::string& fullName, const std::string& state) override {
      return updateResource("/process/ecs/" + fullName + "/state/", Value(state));
    }

    virtual Value getExecutionContextBoundOperationInfos(const std::string& fullName) const override {
      return readResource("/process/ecs/" + fullName + "/operations/");
    }
    
    virtual Value getExecutionContextBoundAllOperationInfos(const std::string& fullName) const override {
      return readResource("/process/ecs/" + fullName + "/all_operations/");
    }

    virtual Value bindOperationToExecutionContext(const std::string& ecFullName, const std::string& opFullName, const Value& opInfo) override {
      return createResource("/process/ecs/" + ecFullName + "/operations/" + opFullName + "/", opInfo);
    }

    virtual Value unbindOperationFromExecutionContext(const std::string& ecFullName, const std::string& opFullName) override {
      return deleteResource("/process/ecs/" + ecFullName + "/operations/" + opFullName + "/");
    }

    virtual Value getBrokerInfos() const override {
      return readResource("/process/brokers/");
    }

    virtual Value getCallbacks() const override {
      return readResource("/process/callbacks/");
    }

    virtual Value getTopicInfos() const override {
      return readResource("/process/topics/");
    }

    virtual Value invokeTopic(const std::string& fullName) const override {
      return readResource("/process/topics/" + fullName + "/");
    }

    virtual Value getTopicConnectionInfos(const std::string& fullName) const override {
      return readResource("/process/topics/" + fullName + "/connections/");
    }

    virtual Value deleteContainer(const std::string& fullName) override {
        return deleteResource("/process/containers/" + fullName + "/");
    }

    virtual Value deleteContainerOperation(const std::string& fullName) override {
        auto [containerName, operationName] = splitContainerAndOperationName(fullName);
        return deleteResource("/process/containers/" + containerName + "/operations/" + operationName + "/");
    }

    virtual Value createExecutionContext(const Value& value) override {
        return createResource("/process/ecs/", value);
    }

    virtual Value deleteExecutionContext(const std::string& fullName) override {
      return deleteResource("/process/ecs/" + fullName + "/");
    }

    virtual Value getFSMInfos() const override {
      return readResource("/process/fsms/");
    }

    virtual Value getFSMInfo(const std::string& fullName) const override {
      return readResource("/process/fsms/" + fullName + "/");
    }

    virtual Value setFSMState(const std::string& fullName, const std::string& state) override {
      return updateResource("/process/fsms/" + fullName + "/state/", {state});
    }

    virtual Value getFSMState(const std::string& fullName) const override {
      return readResource("/process/fsms/" + fullName + "/state/");
    }
    
    virtual Value getOperationsBoundToFSMState(const std::string& fsmFullName, const std::string& state) override {
      return readResource("/process/fsms/" + fsmFullName + "/state/" + state + "/operations/");
    }

    virtual Value getECsBoundToFSMState(const std::string& fsmFullName, const std::string& state) override {
      return readResource("/process/fsms/" + fsmFullName + "/state/" + state + "/fsms/");
    }

    virtual Value getFSMsBoundToFSMState(const std::string& fsmFullName, const std::string& state) override {
      return readResource("/process/fsms/" + fsmFullName + "/state/" + state + "/ecs/");
    }

    virtual Value bindOperationToFSMState(const std::string& fsmFullName, const std::string& state, const Value& operation) override {
      return createResource("/process/fsms/" + fsmFullName + "/state/" + state + "/operations/", operation);
    }

    virtual Value bindECStateToFSMState(const std::string& fsmFullName, const std::string& state, const Value& ecState) override {
      return createResource("/process/fsms/" + fsmFullName + "/state/" + state + "/ecs/", ecState);
    }

    virtual Value bindFSMStateToFSMState(const std::string& fsmFullName, const std::string& state, const Value& targetFsmState) override  {
      return createResource("/process/fsms/" + fsmFullName + "/state/" + state + "/fsms/", targetFsmState);
    }
    */
  };

}