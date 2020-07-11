#pragma once 


#include "nerikiri/brokerapi.h"
#include "nerikiri/brokerfactory.h"

namespace nerikiri {

class Process;

class CoreBroker : public BrokerAPI {
protected:
    Process* process_;

public:

    CoreBroker(Process* process, const Value& info): BrokerAPI(info), process_(process) {}

    virtual ~CoreBroker() {}

    virtual Value getBrokerInfo() const override { return info_; }

    virtual Value getProcessInfo() const override;


    virtual Value getOperationInfos() const override;

    virtual Value getOperationInfo(const std::string& fullName) const override;

    virtual Value callOperation(const std::string& fullName, const Value& value) override;

    virtual Value invokeOperation(const std::string& fullName) const override;

    virtual Value executeOperation(const std::string& fullName) override;

    virtual Value getOperationConnectionInfos(const std::string& fullName) const override;

    virtual Value getOperationFactoryInfos() const override;

    virtual Value createOperation(const Value& value) override;

    virtual Value deleteOperation(const std::string& fullName) override;



    virtual Value getContainerInfos() const override;

    virtual Value getContainerInfo(const std::string& fullName) const override;

    virtual Value getContainerOperationInfos(const std::string& fullName) const override;

    virtual Value createContainer(const Value& value) override;

    virtual Value deleteContainer(const std::string& fullName) override;

    virtual Value getContainerFactoryInfos() const override;



    virtual Value getContainerOperationInfo(const std::string& fullName) const override;

    virtual Value getContainerOperationConnectionInfos(const std::string& fullName) const override;

    virtual Value callContainerOperation(const std::string& fullName, const Value& arg) override;

    virtual Value invokeContainerOperation(const std::string& fullName) const override;

    virtual Value executeContainerOperation(const std::string& fullName) override;

    virtual Value createContainerOperation(const std::string& fullName, const Value& value) override;

    virtual Value deleteContainerOperation(const std::string& fullName) override;



    virtual Value getAllOperationInfos() const override;

    virtual Value getAllOperationInfo(const std::string& fullName) const override;

    virtual Value invokeAllOperation(const std::string& fullName) const override;

    virtual Value executeAllOperation(const std::string& fullName) override;

    virtual Value callAllOperation(const std::string& fullName, const Value& value) override;

    virtual Value getAllOperationConnectionInfos(const std::string& fullName) const override;


    virtual Value getConnectionInfos() const override;

    virtual Value createConnection(const Value& connectionInfo, const Value& brokerInfo) override;

    virtual Value registerConsumerConnection(const Value& ci) override;

    virtual Value registerProviderConnection(const Value& ci) override;

    virtual Value removeConsumerConnection(const std::string& fullName, const std::string& targetArgName, const std::string& conName) override;

    virtual Value removeProviderConnection(const std::string& fullName, const std::string& conName) override;

    virtual Value putToArgument(const std::string& fullName, const std::string& argName, const Value& value) override;

    virtual Value putToArgumentViaConnection(const std::string& fullName, const std::string& targetArgName, const std::string& conName, const Value& value) override;



    virtual Value createResource(const std::string& path, const Value& value) override;

    virtual Value readResource(const std::string& path) const override;

    virtual Value updateResource(const std::string& path, const Value& value) override;

    virtual Value deleteResource(const std::string& path) override;


    virtual Value getExecutionContextInfos() const override;

    virtual Value getExecutionContextInfo(const std::string& fullName) const override;

    virtual Value getExecutionContextState(const std::string& fullName) const override;

    virtual Value setExecutionContextState(const std::string& fullName, const std::string& state) override;

    virtual Value getExecutionContextBoundOperationInfos(const std::string& fullName) const override;

    virtual Value getExecutionContextBoundAllOperationInfos(const std::string& fullName) const override;

    virtual Value bindOperationToExecutionContext(const std::string& ecFullName, const std::string& opFullName, const Value& opInfo) override;

    virtual Value unbindOperationFromExecutionContext(const std::string& ecFullName, const std::string& opFullName) override;

    virtual Value getBrokerInfos() const override;

    virtual Value getCallbacks() const override;

    virtual Value getTopicInfos() const override;

    virtual Value invokeTopic(const std::string& fullName) const override;
    
    virtual Value getTopicConnectionInfos(const std::string& fullName) const override;

    virtual Value getExecutionContextFactoryInfos() const override;

    virtual Value createExecutionContext(const Value& value) override;

    virtual Value deleteExecutionContext(const std::string& fullName) override;


    virtual Value getFSMInfos() const override;
};


class CoreBrokerFactory : public nerikiri::BrokerFactory {
private:
  std::shared_ptr<BrokerAPI> coreBroker_;

public:
  CoreBrokerFactory(std::shared_ptr<BrokerAPI> coreBroker): BrokerFactory({{"typeName", "CoreBroker"}}), coreBroker_(coreBroker) {}
  virtual ~CoreBrokerFactory() {}

public:
  virtual std::shared_ptr<Broker> create(const Value& param) { return nullptr; }
  virtual std::shared_ptr<BrokerAPI> createProxy(const Value& param) {
      return coreBroker_;
  }
};

}