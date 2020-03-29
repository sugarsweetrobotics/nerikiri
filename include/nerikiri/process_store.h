#pragma once
#include <vector>
#include "nerikiri/operation.h"
#include "nerikiri/container.h"
#include "nerikiri/ec.h"
#include "nerikiri/connection.h"
#include "nerikiri/connectiondictionary.h"

namespace nerikiri {


  class Process;

  class ProcessStore {
  private:
    Process* process_;
    std::vector<std::shared_ptr<Operation>> operations_;
    ConnectionDictionary connectionDictionary_;

    std::vector<std::shared_ptr<ContainerBase>> containers_;
    std::vector<std::shared_ptr<ExecutionContext>> executionContexts_;

  public:
    ProcessStore(Process* process): process_(process) {}

  public:
    Value info() const;
    Value getContainerInfos();
    ContainerBase& getContainerByName(const std::string& name);
    Value addContainer(std::shared_ptr<ContainerBase> container);

    Value getOperationInfos();
    OperationBaseBase& getOperation(const OperationInfo& oi);
    Value addOperation(std::shared_ptr<Operation> op);
    
    Value addConnection(Connection_ptr con);
    Value getConnectionInfos() const ;
  };
}