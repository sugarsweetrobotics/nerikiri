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

    virtual Value getContainerInfos() const override;

    virtual Value getContainerInfo(const Value& info) const override;

    virtual Value getContainerOperationInfos(const Value& info) const override;

    virtual Value getContainerOperationInfo(const Value& cinfo, const Value& oinfo) const override;

    virtual Value callContainerOperation(const Value& cinfo, const Value& oinfo, Value&& arg) override;

    virtual Value invokeContainerOperation(const Value& cinfo, const Value& oinfo) const override;

    virtual Value getOperationInfo(const Value& info) const override;

    virtual Value callOperation(const Value& info, Value&& value) override;

    virtual Value invokeOperation(const Value& v) const override;

    virtual Value executeOperation(const Value& v) override;

    virtual Value getConnectionInfos() const override;

    virtual Value registerConsumerConnection(const Value& ci) override;

    virtual Value registerProviderConnection(const Value& ci) override;

    virtual Value removeConsumerConnection(const Value& ci) override;

    virtual Value removeProviderConnection(const Value& ci) override;

    virtual Value putToArgument(const Value& opInfo, const std::string& argName, const Value& value) override;

    virtual Value putToArgumentViaConnection(const Value& conInfo, const Value& value) override;
    
    virtual Value getOperationFactoryInfos() const override;

    virtual Value getContainerFactoryInfos() const override;

    virtual Value createOperation(const Value& value) override;

    virtual Value createContainer(const Value& value) override;

    virtual Value createResource(const std::string& path, const Value& value) override;

    virtual Value readResource(const std::string& path) const override;

    virtual Value updateResource(const std::string& path, const Value& value) override;

    virtual Value deleteResource(const std::string& path) override;

    virtual Value createContainerOperation(const Value& containerInfo, const Value& value) override;

    virtual Value deleteOperation(const Value& value) override;

    virtual Value deleteContainer(const Value& value) override;

    virtual Value deleteContainerOperation(const Value& containerInfo, const Value& value) override;

    virtual Value createExecutionContext(const Value& value) override;

    virtual Value deleteExecutionContext(const Value& value) override;

};


class CoreBrokerFactory : public nerikiri::BrokerFactory {
private:
  std::shared_ptr<BrokerAPI> coreBroker_;

public:
  CoreBrokerFactory(std::shared_ptr<BrokerAPI> coreBroker): BrokerFactory({{"name", "CoreBroker"}}), coreBroker_(coreBroker) {}
  virtual ~CoreBrokerFactory() {}

public:
  virtual std::shared_ptr<Broker> create(const Value& param) { return nullptr; }
  virtual std::shared_ptr<BrokerAPI> createProxy(const Value& param) {
      return coreBroker_;
  }
};

}