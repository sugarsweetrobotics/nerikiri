#pragma once

#include "nerikiri/operation.h"
#include "nerikiri/value.h"
#include "nerikiri/broker.h"

#include "nerikiri/runnable.h"
#include "nerikiri/systemeditor.h"
#include "nerikiri/container.h"
#include "nerikiri/ec.h"
#include "nerikiri/process_store.h"

namespace nerikiri {

 using ProcessInfo = Value;

  class Process {
  private:
    ProcessInfo info_;

    std::map<std::string, SystemEditor_ptr> systemEditors_;
    std::vector<std::thread> threads_;
    std::vector<Broker_ptr> brokers_;

    ProcessStore store_;
  public:
    Process(const std::string& name);
    ~Process();

    static Process null;
    
  public:
    Value getOperationInfos() { return store_.getOperationInfos(); }
    Process& addOperation(Operation&& op) { store_.addOperation(std::move(op)); return *this; }
    Process& addOperation(const Operation& op) {store_.addOperation((op)); return *this; }
    OperationBaseBase& getOperation(const OperationInfo& oi) { return store_.getOperation(oi); }

    Value getContainerInfos() {return store_.getContainerInfos(); }
    ContainerBase& getContainerByName(const std::string& name) { return store_.getContainerByName(name); }
    Process& addContainer(std::shared_ptr<ContainerBase> container) { store_.addContainer(container); return *this; }

    Process& addBroker(Broker_ptr&& brk);
    Broker_ptr getBrokerByName(const std::string& name);
    Broker_ptr getBrokerByInfo(const BrokerInfo& ci);
    
    Process& addSystemEditor(SystemEditor_ptr&& se);
    Process& addConnection(Connection_ptr&& con);


    Process& addExecutionContext(const ExecutionContext& ec);

    int32_t start();
    void startAsync();
    int32_t wait();
    void shutdown();
    
    ProcessInfo info() const { return info_; }
    
  public:
    Value invokeConnection(const Connection& con);


  public:
    void executeOperation(const OperationInfo& oinfo) const;
  };


  //  inline Process process(std::vector<std::string> args) { return std::move(Process(args.size() > 0 ? args[0] : "default_process_name")); }

}
