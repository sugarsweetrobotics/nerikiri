#pragma once 


#include <nerikiri/broker_api.h>
#include <nerikiri/broker_proxy_api.h>
#include <nerikiri/broker_factory_api.h>
#include <nerikiri/broker_factory.h>

namespace nerikiri {

class Process;

class CoreFactoryBroker : public FactoryBrokerAPI {
private:
  Process* process_;
public:
  CoreFactoryBroker(Process* proc) : process_(proc) {}
  virtual ~CoreFactoryBroker() {}

  virtual Value createObject(const std::string& className, const Value& info={}) override;

  virtual Value deleteObject(const std::string& className, const std::string& fullName) override;
};

class CoreStoreBroker : public StoreBrokerAPI {
private:
  Process* process_;
public:
  CoreStoreBroker(Process* proc) : process_(proc) {}
  virtual ~CoreStoreBroker() {}

  virtual Value getClassObjectInfos(const std::string& className) const override;

  virtual Value getChildrenClassObjectInfos(const std::string& parentName, const std::string& className) const override;

  virtual Value getObjectInfo(const std::string& className, const std::string& fullName) const override;
};

class CoreOperationBroker : public OperationBrokerAPI {
private:
  Process* process_;
public:
  CoreOperationBroker(Process* proc) : process_(proc) {}
  virtual ~CoreOperationBroker() {}

  virtual Value fullInfo(const std::string& fullName) const override;

  virtual Value call(const std::string& fullName, const Value& value) override;

  virtual Value invoke(const std::string& fullName) override;

  virtual Value execute(const std::string& fullName) override;

  virtual Value inlets(const std::string& fullName) const override;

};


class CoreOperationOutletBroker : public OperationOutletBrokerAPI {
public:
  CoreOperationOutletBroker() {}
  virtual ~CoreOperationOutletBroker() {}

  virtual Value get(const std::string& fullName) const override;

  virtual Value connections(const std::string& fullName) const override;

  virtual Value addConnection(const std::string& fullName, const Value& c) override;
  
  virtual Value removeConnection(const std::string& fullName, const std::string& name) override;
};

class CoreOperationInletBroker : public OperationInletBrokerAPI {
public:
  CoreOperationInletBroker() {}
  virtual ~CoreOperationInletBroker() {}

  virtual Value name(const std::string& fullName, const std::string& targetName) const override;
  
  virtual Value defaultValue(const std::string& fullName, const std::string& targetName) const override;

  virtual Value put(const std::string& fullName, const std::string& targetName, const Value& value) const override;

  virtual Value get(const std::string& fullName, const std::string& targetName) const override;

  virtual Value isUpdated(const std::string& fullName, const std::string& targetName) const override;

  virtual Value connections(const std::string& fullName, const std::string& targetName) const override;

  virtual Value addConnection(const std::string& fullName, const std::string& targetName, const Value& c) override;
  
  virtual Value removeConnection(const std::string& fullName, const std::string& targetName, const std::string& name) override;
};

class CoreBroker : public BrokerProxyAPI {
protected:
  Process* process_;
public:
  std::shared_ptr<CoreFactoryBroker> factory_;
  std::shared_ptr<CoreStoreBroker> store_;
  std::shared_ptr<CoreOperationBroker> operation_;
  std::shared_ptr<CoreOperationOutletBroker> operationOutlet_;
  std::shared_ptr<CoreOperationInletBroker> operationInlet_;
public:

  virtual std::shared_ptr<FactoryBrokerAPI> factory() override { return factory_; }
  virtual std::shared_ptr<StoreBrokerAPI>  store() override { return store_; }
  virtual std::shared_ptr<const FactoryBrokerAPI> factory() const override{ return factory_; }
  virtual std::shared_ptr<const StoreBrokerAPI> store() const override{ return store_; }
  virtual std::shared_ptr<OperationBrokerAPI>   operation() override { return operation_; }
  virtual std::shared_ptr<const OperationBrokerAPI>   operation() const override { return operation_; }
  virtual std::shared_ptr<OperationOutletBrokerAPI>   operationOutlet() override { return operationOutlet_; }
  virtual std::shared_ptr<const OperationOutletBrokerAPI>   operationOutlet() const override { return operationOutlet_; }
  virtual std::shared_ptr<OperationInletBrokerAPI>   operationInlet() override { return operationInlet_; }
  virtual std::shared_ptr<const OperationInletBrokerAPI>   operationInlet() const override { return operationInlet_; }

public:

  /**
   * 
   */
  CoreBroker(Process* process, const std::string& fullName): BrokerProxyAPI("CoreBroker", fullName), process_(process),
   factory_(std::make_shared<CoreFactoryBroker>(process)), 
   store_(std::make_shared<CoreStoreBroker>(process)) {}

  virtual ~CoreBroker() {}

    virtual Value getProcessInfo() const override;
    virtual Value getProcessFullInfo() const override;
  /*
    virtual Value getBrokerInfo() const override { return info_; }



    virtual Value getOperationInfos() const override;

    virtual Value getOperationInfo(const std::string& fullName) const override;

    virtual Value callOperation(const std::string& fullName, const Value& value) override;

    virtual Value invokeOperation(const std::string& fullName) const override;

    virtual Value executeOperation(const std::string& fullName) override;

    virtual Value getOperationConnectionInfos(const std::string& fullName) const override;

    virtual Value getOperationFactoryInfos() const override;



    virtual Value getContainerInfos() const override;

    virtual Value getContainerInfo(const std::string& fullName) const override;

    virtual Value getContainerOperationInfos(const std::string& fullName) const override;


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
    
    virtual Value getFSMInfo(const std::string& fullName) const override;

    virtual Value setFSMState(const std::string& fullName, const std::string& state) override;
    
    virtual Value getFSMState(const std::string& fullName) const override;

    virtual Value getOperationsBoundToFSMState(const std::string& fsmFullName, const std::string& state) override;

    virtual Value getECsBoundToFSMState(const std::string& fsmFullName, const std::string& state) override;

    virtual Value getFSMsBoundToFSMState(const std::string& fsmFullName, const std::string& state) override;

    virtual Value bindOperationToFSMState(const std::string& fsmFullName, const std::string& state, const Value& operation) override;

    virtual Value bindECStateToFSMState(const std::string& fsmFullName, const std::string& state, const Value& ecState) override;

    virtual Value bindFSMStateToFSMState(const std::string& fsmFullName, const std::string& state, const Value& fsmState) override;
 */
};


class CoreBrokerFactory : public nerikiri::BrokerFactoryAPI {
private:
  std::shared_ptr<CoreBroker> coreBroker_;

public:
  //virtual std::string brokerTypeFullName() const override {
  //    return "CoreBroker";
  //}


  CoreBrokerFactory(const std::shared_ptr<CoreBroker>& coreBroker):
   BrokerFactoryAPI("CoreBrokerFactory", "CoreBroker", "coreBrokerFactory"), coreBroker_(coreBroker) {}
  virtual ~CoreBrokerFactory() {}

public:
  virtual std::shared_ptr<BrokerAPI> create(const Value& param) override { 
    return std::make_shared<NullBroker>(); 
  }

  virtual std::shared_ptr<BrokerProxyAPI> createProxy(const Value& param) override {
      return coreBroker_;
  }
};

}