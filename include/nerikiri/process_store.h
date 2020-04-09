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
    
  public:
    ProcessStore(Process* process): process_(process) {}

  public:
    Value info() const;
    Value getContainerInfos();
    ContainerBase& getContainer(const Value& info);
    //ContainerBase& getContainerByName(const std::string& name);
    Value addContainer(std::shared_ptr<ContainerBase> container);
    ProcessStore& addContainerFactory(std::shared_ptr<ContainerFactoryBase> cf);
    std::shared_ptr<ContainerFactoryBase> getContainerFactory(const Value& info);


    Value getOperationInfos();
    Value getOperationFactoryInfos() {
      return nerikiri::map<Value, std::shared_ptr<OperationFactory>>(operationFactories_, [](auto& opf) { return Value(opf->typeName()); });
    }
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
    Value getExecutionContextInfos() {
      return nerikiri::map<Value, std::shared_ptr<ExecutionContext>>(executionContexts_, [](auto& ec) { return Value(ec->info()); });
    }
    Value getExecutionContextFactoryInfos() {
      return nerikiri::map<Value, std::shared_ptr<ExecutionContextFactory>>(executionContextFactories_, [](auto& ecf) { return Value(ecf->typeName()); });
    }

    std::shared_ptr<ExecutionContext> getExecutionContext(const Value& info) {
      for(auto& ec : executionContexts_) {
        if (ec->info().at("instanceName") == info.at("instanceName")) {
          return ec;
        }
      }
      return nullptr;
    }
  };
}