#pragma once

#include "nerikiri/operation.h"
#include "nerikiri/value.h"
#include "nerikiri/broker.h"
#include "nerikiri/connection.h"
#include "nerikiri/connectiondictionary.h"

#include "nerikiri/runnable.h"
#include "nerikiri/systemeditor.h"
#include "nerikiri/container.h"

namespace nerikiri {

 using ProcessInfo = Value;
  
  class Process {
  private:
    //BrokerDictionary brokerDictionary_;
    ConnectionDictionary connectionDictionary_;
    std::map<std::string, SystemEditor_ptr> systemEditors_;
    std::vector<std::thread> threads_;
    ProcessInfo info_;
    std::vector<Operation> operations_;
    std::vector<Broker_ptr> brokers_;

    std::vector<ContainerBase*> containers_;
    std::vector<ContainerOperationBase*> containerOperations_;
  public:
    Process(const std::string& name);
    ~Process();

    static Process null;
    
  public:
    Process& addOperation(Operation&& op);
    Process& addOperation(const Operation& op);
    OperationBaseBase& getOperation(const OperationInfo& oi);
    
    Process& addBroker(Broker_ptr&& brk);
    Broker_ptr getBrokerByName(const std::string& name);

    Broker_ptr getBrokerByInfo(const BrokerInfo& ci);
    
    Process& addSystemEditor(SystemEditor_ptr&& se);
    Process& addConnection(Connection_ptr&& con);

    Value getContainerInfos();
    ContainerBase& getContainerByName(const std::string& name);
    Process& addContainer(ContainerBase* container);
    Process& addOperationToContainerByName(const std::string& name, ContainerOperationBase* operation);

    int32_t start();
    void startAsync();
    int32_t wait();
    void shutdown();
    ProcessInfo info() const { return info_; }
    
    Value getOperationInfos();

  public:
    Value invokeConnection(const Connection& con);


  public:
    void executeOperation(const OperationInfo& oinfo) const;
  };


  //  inline Process process(std::vector<std::string> args) { return std::move(Process(args.size() > 0 ? args[0] : "default_process_name")); }

}
