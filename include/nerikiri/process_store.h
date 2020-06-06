#pragma once
#include <vector>
#include "nerikiri/nerikiri.h"
#include "nerikiri/util/dllproxy.h"


#include "nerikiri/operation.h"
#include "nerikiri/containers/container.h"
#include "nerikiri/ec.h"
#include "nerikiri/connection.h"
#include "nerikiri/connectiondictionary.h"
#include "nerikiri/operationfactory.h"

//#include "nerikiri/brokers/corebroker.h"
#include "nerikiri/brokers/brokerfactory.h"
#include "nerikiri/topic.h"

namespace nerikiri {


  class Process;

  class NK_API ProcessStore {
  private:
    Process* process_;

    std::vector<std::shared_ptr<DLLProxy>> dllproxies_;

    std::vector<std::shared_ptr<Operation>> operations_;
    std::vector<std::shared_ptr<OperationFactory>> operationFactories_;

    std::vector<std::shared_ptr<ContainerBase>> containers_;
    std::vector<std::shared_ptr<ContainerFactoryBase>> containerFactories_;

    std::vector<std::shared_ptr<ExecutionContext>> executionContexts_;
    std::vector<std::shared_ptr<ExecutionContextFactory>> executionContextFactories_;
    
    std::vector<std::shared_ptr<Broker>> brokers_;
    std::vector<std::shared_ptr<BrokerFactory>> brokerFactories_;

    std::vector<std::shared_ptr<Topic>> topics_;
    std::vector<std::shared_ptr<TopicFactory>> topicFactories_;
    friend class Process;
  public:
    ProcessStore(Process* process);
    ~ProcessStore();
  public:
    Value info() const;
    Value getContainerInfos();
    Value getContainerFactoryInfos();
    std::shared_ptr<ContainerBase> getContainer(const Value& info);
    std::vector<std::shared_ptr<ContainerBase>> getContainers() { return containers_; }
    Value addContainer(std::shared_ptr<ContainerBase> container);
    ProcessStore& addContainerFactory(std::shared_ptr<ContainerFactoryBase> cf);
    std::shared_ptr<ContainerFactoryBase> getContainerFactory(const Value& info);


    Value getOperationInfos();
    Value getOperationFactoryInfos();
    std::shared_ptr<OperationBase> getOperation(const Value& oi);
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

    Value addBroker(std::shared_ptr<Broker> brk);
    std::shared_ptr<Broker> getBroker(const Value& info);
    Value addBrokerFactory(std::shared_ptr<BrokerFactory> factory);
    Value getBrokerInfos() const;
    std::shared_ptr<BrokerFactory> getBrokerFactory(const Value& info);

    Value addDLLProxy(std::shared_ptr<DLLProxy> dllproxy);

    Value getCallbacks() const;

    std::shared_ptr<TopicFactory> getTopicFactory(const Value& topicInfo);
    Value addTopicFactory(std::shared_ptr<TopicFactory> tf);
    std::shared_ptr<Topic> getTopic(const Value& topicInfo);
    Value getTopicInfos() const;
    Value addTopic(std::shared_ptr<Topic> topic);

    std::shared_ptr<OperationBase> getOperationOrTopic(const Value& info);
  };
}