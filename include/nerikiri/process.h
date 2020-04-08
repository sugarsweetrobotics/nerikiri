#pragma once

#include "nerikiri/operation.h"
#include "nerikiri/value.h"
#include "nerikiri/broker.h"

#include "nerikiri/runnable.h"
#include "nerikiri/systemeditor.h"
#include "nerikiri/container.h"
#include "nerikiri/ec.h"
#include "nerikiri/process_store.h"
#include "nerikiri/objectfactory.h"
#include "nerikiri/objectmapper.h"

namespace nerikiri {

 using ProcessInfo = Value;


  /**
   * プロセスクラス
   */
  class Process {
  private:
    ProcessInfo info_;

    std::map<std::string, SystemEditor_ptr> systemEditors_;
    std::vector<std::thread> threads_;
    ObjectFactory objectFactory_;
    ProcessStore store_;

    std::vector<Broker_ptr> brokers_;
    std::vector<std::shared_ptr<BrokerFactory>> brokerFactories_;

    static Process null;
  public:

    /**
     * コンストラクタ
     * @param name
     */
    Process(const std::string& name);

    /**
     * デストラクタ
     */
    ~Process();
   
  public:
    ProcessStore* store() { return &store_; }

  public:
    Value getOperationInfos() { return store_.getOperationInfos(); }
    Value getOperationFactoryInfos() { return store_.getOperationFactoryInfos(); }
    Process& addOperation(std::shared_ptr<Operation> op) { store_.addOperation(op); return *this; }
    Process& addOperationFactory(std::shared_ptr<OperationFactory> opf) { store_.addOperationFactory(opf); return *this; }
    Value createOperation(const OperationInfo& info);
    OperationBaseBase& getOperation(const OperationInfo& oi) { return store_.getOperation(oi); }

    Value getContainerInfos() {return store_.getContainerInfos(); }
    ContainerBase& getContainerByName(const std::string& name) { return store_.getContainerByName(name); }
    Process& addContainer(std::shared_ptr<ContainerBase> container) { store_.addContainer(container); return *this; }
    Value createContainer(const Value& ci);
    Process& addContainerFactory(std::shared_ptr<ContainerFactoryBase> cf) { store_.addContainerFactory(cf); return *this; }

    Process& addContainerOperationFactory(std::shared_ptr<ContainerOperationFactoryBase> cof) { store_.addContainerOperationFactory(cof); return *this; }
    Value createContainerOperation(const Value& containerInfo, const Value& operationInfo) {
      return store_.getContainer(containerInfo).createContainerOperation(operationInfo);
    }

    Value addBroker(const Broker_ptr& brk);
    Broker_ptr getBrokerByName(const std::string& name);
    Broker_ptr createBrokerProxy(const BrokerInfo& ci);
    Value createBroker(const BrokerInfo& ci);
    Process& addBrokerFactory(std::shared_ptr<BrokerFactory> factory) { brokerFactories_.push_back(factory); return *this;}
    
    Process& addSystemEditor(SystemEditor_ptr&& se);
    Process& addConnection(Connection_ptr&& con);

    Value putToArgument(const Value& opInfo, const std::string& argName, const Value& value);

    Value putToArgumentViaConnection(const Value& conInfo, const Value& value);

    Value getExecutionContextInfos() { return store_.getExecutionContextInfos(); }
    Value getExecutionCOntextFactoryInfos() { return store_.getExecutionContextFactoryInfos(); }
    Process& addExecutionContext(std::shared_ptr<ExecutionContext> ec) { store_.addExecutionContext(ec); return *this; }
    Process& addExecutionContextFactory(std::shared_ptr<ExecutionContextFactory> ec) { store_.addExecutionContextFactory(ec); return *this; }
    Value createExecutionContext(const Value& value);


    int32_t start();
    void startAsync();
    int32_t wait();
    void shutdown();
    
    ProcessInfo info() const { return info_; }
    

  public:
    std::function<void(Process*)> on_starting_;
    std::function<void(Process*)> on_started_;

  public:
    Process& setOnStarting(std::function<void(Process*)> f) {
      on_starting_ = f;
      return *this;
    }
    Process& setOnStarted(std::function<void(Process*)> f) {
      on_started_ = f;
      return *this;
    }
  public:
    //Value makeConnection(const ConnectionInfo& ci, BrokerAPI* receiverBroker=nullptr);
    Value invokeConnection(const Connection& con);
    Value registerConsumerConnection(const ConnectionInfo& ci);
    Value registerProviderConnection(const ConnectionInfo& ci, BrokerAPI* receiverBroker=nullptr);
  
    Value deleteConsumerConnection(const ConnectionInfo& ci);
    Value deleteProviderConnection(const ConnectionInfo& ci);

  public:
    void executeOperation(const OperationInfo& oinfo) const;

    Value bindECtoOperation(const std::string& ecName, const Value& opInfo);

    Value requestResource(const std::string& path) {
      return nerikiri::ObjectMapper::requestResource(this->store(), path);
    }

    Value createResource(const std::string& path, const Value& value, BrokerAPI* receiverBroker = nullptr) {
      return nerikiri::ObjectMapper::createResource(this, path, value, receiverBroker);
    }

    Value writeResource(const std::string& path, const Value& value, BrokerAPI* receiverBroker = nullptr) {
      return nerikiri::ObjectMapper::writeResource(this, path, value, receiverBroker);
    }

    Value deleteResource(const std::string& path, BrokerAPI* receiverBroker = nullptr) {
      return nerikiri::ObjectMapper::deleteResource(this, path, receiverBroker);
    }
  };


  //  inline Process process(std::vector<std::string> args) { return std::move(Process(args.size() > 0 ? args[0] : "default_process_name")); }

}
