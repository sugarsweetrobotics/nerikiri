#pragma once

#include "nerikiri/operation.h"
#include "nerikiri/value.h"
#include "nerikiri/brokers/brokerapi.h"
#include "nerikiri/brokers/corebroker.h"

#include "nerikiri/systemeditor.h"
#include "nerikiri/container.h"
#include "nerikiri/containerfactory.h"
#include "nerikiri/containeroperation.h"
#include "nerikiri/containeroperationfactory.h"
#include "nerikiri/ec.h"
#include "nerikiri/process_store.h"
#include "nerikiri/objectfactory.h"
#include "nerikiri/operationfactory.h"
#include "nerikiri/objectmapper.h"

#include "nerikiri/dllproxy.h"

namespace nerikiri {

 using ProcessInfo = Value;


  /**
   * プロセスクラス
   */
  class Process {
  private:

    std::vector<std::shared_ptr<DLLProxy>> dllproxies_;
    
    
    ProcessInfo info_;
    Value config_;

    
    std::map<std::string, SystemEditor_ptr> systemEditors_;
    std::vector<std::thread> threads_;
    ObjectFactory objectFactory_;
    ProcessStore store_;
    std::shared_ptr<CoreBroker> coreBroker_;

    bool started_;
    static Process null;

  public:

    /**
     * コンストラクタ
     * @param name
     */
    Process(const std::string& name);


    Process(const int argc, const char** argv);

    /**
     * デストラクタ
     */
    ~Process();
  private:
    void _preloadOperations();
    void _preloadContainers();
    void _preloadExecutionContexts();
    void _preStartExecutionContexts();
    void _preloadBrokers();
  public:
    ProcessStore* store() { return &store_; }
  private: 
    std::string path_;
    void setExecutablePath(const std::string& path) { path_ = path; }
  public:
    Value getOperationInfos() { return store_.getOperationInfos(); }
    Value getOperationFactoryInfos() { return store_.getOperationFactoryInfos(); }
    Process& addOperation(std::shared_ptr<Operation> op) { store_.addOperation(op); return *this; }
    Process& addOperationFactory(std::shared_ptr<OperationFactory> opf) { store_.addOperationFactory(opf); return *this; }
    Value createOperation(const OperationInfo& info);
    OperationBaseBase& getOperation(const OperationInfo& oi) { return store_.getOperation(oi); }
    Value loadOperationFactory(const Value& info);

    Value getContainerInfos() {return store_.getContainerInfos(); }
    ContainerBase& getContainer(const Value& info) { return store_.getContainer(info); }
    Process& addContainer(std::shared_ptr<ContainerBase> container) { store_.addContainer(container); return *this; }
    Value createContainer(const Value& ci);
    Process& addContainerFactory(std::shared_ptr<ContainerFactoryBase> cf) { store_.addContainerFactory(cf); return *this; }
    Value loadContainerFactory(const Value& info);

    Process& addContainerOperationFactory(std::shared_ptr<ContainerOperationFactoryBase> cof) { store_.addContainerOperationFactory(cof); return *this; }
    Value createContainerOperation(const Value& containerInfo, const Value& operationInfo) {
      return store_.getContainer(containerInfo).createContainerOperation(operationInfo);
    }
    Value loadContainerOperationFactory(const Value& info);

    Process& addBroker(const std::shared_ptr<Broker> brk) { store_.addBroker(brk, this); return *this; }
    Value createBroker(const Value& ci);
    std::shared_ptr<BrokerAPI>  createBrokerProxy(const Value& ci);
    Process& addBrokerFactory(std::shared_ptr<BrokerFactory> factory) { store_.addBrokerFactory(factory); return *this;}
    Value loadBrokerFactory(const Value& info);


    Process& addSystemEditor(SystemEditor_ptr&& se);
    Process& addConnection(Connection_ptr&& con);

    Value putToArgument(const Value& opInfo, const std::string& argName, const Value& value);

    Value putToArgumentViaConnection(const Value& conInfo, const Value& value);

    Value getExecutionContextInfos() { return store_.getExecutionContextInfos(); }
    Value getExecutionCOntextFactoryInfos() { return store_.getExecutionContextFactoryInfos(); }
    Process& addExecutionContext(std::shared_ptr<ExecutionContext> ec) { store_.addExecutionContext(ec); return *this; }
    Process& addExecutionContextFactory(std::shared_ptr<ExecutionContextFactory> ec) { store_.addExecutionContextFactory(ec); return *this; }
    Value createExecutionContext(const Value& value);
    Value loadExecutionContextFactory(const Value& info);


    int32_t start();
    void startAsync();
    int32_t wait();
    void shutdown();
    
    ProcessInfo info() const { return info_; }
    
  public:
    bool isRunning() { return started_; }
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
    Value executeOperation(const OperationInfo& oinfo);

    Value bindECtoOperation(const std::string& ecName, const Value& opInfo);

    Value readResource(const std::string& path) {
      return nerikiri::ObjectMapper::readResource(this->store(), path);
    }

    Value createResource(const std::string& path, const Value& value) {
      return nerikiri::ObjectMapper::createResource(this, path, value);
    }

    Value updateResource(const std::string& path, const Value& value) {
      return nerikiri::ObjectMapper::updateResource(this, path, value);
    }

    Value deleteResource(const std::string& path) {
      return nerikiri::ObjectMapper::deleteResource(this, path);
    }

    void parseConfigFile(const std::string& filepath);
  };


  //  inline Process process(std::vector<std::string> args) { return std::move(Process(args.size() > 0 ? args[0] : "default_process_name")); }

}
