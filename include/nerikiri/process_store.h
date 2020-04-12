#pragma once
#include <vector>
#include "nerikiri/operation.h"
#include "nerikiri/container.h"
#include "nerikiri/ec.h"
#include "nerikiri/connection.h"
#include "nerikiri/connectiondictionary.h"
#include "nerikiri/operationfactory.h"
namespace nerikiri {


  class Process;

  class ProcessStore {
  private:
    Process* process_;
    std::vector<std::shared_ptr<Operation>> operations_;
    std::vector<std::shared_ptr<OperationFactory>> operationFactories_;
    ConnectionDictionary connectionDictionary_;

    std::vector<std::shared_ptr<ContainerBase>> containers_;
    std::vector<std::shared_ptr<ContainerFactoryBase>> containerFactories_;

    std::vector<std::shared_ptr<ExecutionContext>> executionContexts_;
    std::vector<std::shared_ptr<ExecutionContextFactory>> executionContextFactories_;
    

    std::vector<std::shared_ptr<Broker>> brokers_;
    std::vector<std::shared_ptr<BrokerFactory>> brokerFactories_;


    friend class Process;
  public:
    ProcessStore(Process* process): process_(process) {}

  public:
    Value info() const;
    Value getContainerInfos();
    ContainerBase& getContainer(const Value& info);
    Value addContainer(std::shared_ptr<ContainerBase> container);
    ProcessStore& addContainerFactory(std::shared_ptr<ContainerFactoryBase> cf);
    std::shared_ptr<ContainerFactoryBase> getContainerFactory(const Value& info);


    Value getOperationInfos();
    Value getOperationFactoryInfos();
    OperationBaseBase& getOperation(const OperationInfo& oi);
    Value addOperation(std::shared_ptr<Operation> op);
    ProcessStore& addOperationFactory(std::shared_ptr<OperationFactory> opf);
    std::shared_ptr<OperationFactory> getOperationFactory(const Value& info);
    
    Value addConnection(Connection_ptr con);
    Value getConnectionInfos() const ;

    Value addContainerOperationFactory(std::shared_ptr<ContainerOperationFactoryBase> cof);

    Value addExecutionContext(std::shared_ptr<ExecutionContext> ec);
    ProcessStore& addExecutionContextFactory(std::shared_ptr<ExecutionContextFactory> ec);
    std::shared_ptr<ExecutionContextFactory> getExecutionContextFactory(const Value& info);
    Value getExecutionContextInfos();
    Value getExecutionContextFactoryInfos();
    std::shared_ptr<ExecutionContext> getExecutionContext(const Value& info);

    Value addBroker(std::shared_ptr<Broker> brk, Process* process);
    std::shared_ptr<Broker> getBroker(const Value& info);
    Value addBrokerFactory(std::shared_ptr<BrokerFactory> factory);


  };
}