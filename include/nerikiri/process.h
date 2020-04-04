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

  class Process {
  private:
    ProcessInfo info_;

    std::map<std::string, SystemEditor_ptr> systemEditors_;
    std::vector<std::thread> threads_;
    std::vector<Broker_ptr> brokers_;
    ObjectFactory objectFactory_;
    ProcessStore store_;
    std::vector<std::shared_ptr<BrokerFactory>> brokerFactories_;
  public:
    Process(const std::string& name);
    ~Process();

    static Process null;
    
  public:
    ProcessStore* store() { return &store_; }
    Value getOperationInfos() { return store_.getOperationInfos(); }
    Process& addOperation(std::shared_ptr<Operation> op) { store_.addOperation(op); return *this; }
    //Process& addOperation(const Operation& op) {store_.addOperation((op)); return *this; }
    OperationBaseBase& getOperation(const OperationInfo& oi) { return store_.getOperation(oi); }

    Value getContainerInfos() {return store_.getContainerInfos(); }
    ContainerBase& getContainerByName(const std::string& name) { return store_.getContainerByName(name); }
    Process& addContainer(std::shared_ptr<ContainerBase> container) { store_.addContainer(container); return *this; }

    Process& addBroker(const Broker_ptr& brk);
    Broker_ptr getBrokerByName(const std::string& name);
    Broker_ptr createBrokerProxy(const BrokerInfo& ci);
    Process& createBroker(const BrokerInfo& ci);
    Process& addBrokerFactory(std::shared_ptr<BrokerFactory> factory) { brokerFactories_.push_back(factory); return *this;}
    
    Process& addSystemEditor(SystemEditor_ptr&& se);
    Process& addConnection(Connection_ptr&& con);

    Value putToArgument(const Value& opInfo, const std::string& argName, const Value& value);


    Process& addExecutionContext(const ExecutionContext& ec);

    int32_t start();
    void startAsync();
    int32_t wait();
    void shutdown();
    
    ProcessInfo info() const { return info_; }
    
  public:
    //Value makeConnection(const ConnectionInfo& ci, BrokerAPI* receiverBroker=nullptr);
    Value invokeConnection(const Connection& con);
    Value registerConsumerConnection(const ConnectionInfo& ci);
    Value registerProviderConnection(const ConnectionInfo& ci, BrokerAPI* receiverBroker=nullptr);
  
    Value deleteConsumerConnection(const ConnectionInfo& ci);
    Value deleteProviderConnection(const ConnectionInfo& ci);

  public:
    void executeOperation(const OperationInfo& oinfo) const;

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
