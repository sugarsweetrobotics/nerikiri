#pragma once

#include "nerikiri/nerikiri.h"
#include "nerikiri/object.h"
#include "nerikiri/operation.h"
#include "nerikiri/value.h"
#include "nerikiri/brokers/brokerapi.h"
#include "nerikiri/brokers/broker.h"
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

 //using ProcessInfo = Value;


  /**
   * プロセスクラス
   */
  class NK_API Process : public Object {
  private:

    std::vector<std::shared_ptr<DLLProxy>> dllproxies_;
    
    Value config_;

    
    std::map<std::string, SystemEditor_ptr> systemEditors_;
    std::vector<std::shared_ptr<std::thread>> threads_;
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

    Process(const std::string& name, const Value& config);

    Process(const std::string& name, const std::string& jsonStr);
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
    void _preloadConnections();

    void _setupLogger();
  public:
    ProcessStore* store() { return &store_; }
  private: 
    std::string path_;
    void setExecutablePath(const std::string& path) { path_ = path; }
  public:
    Value createOperation(const Value& info);
    Value loadOperationFactory(const Value& info);

    Value createContainer(const Value& ci);
    Value loadContainerFactory(const Value& info);
    Value loadContainerOperationFactory(const Value& info);

    Value createBroker(const Value& ci);
    std::shared_ptr<BrokerAPI>  createBrokerProxy(const Value& ci);
    Value loadBrokerFactory(const Value& info);

    Value createExecutionContext(const Value& value);
    Value loadExecutionContextFactory(const Value& info);

    Process& addSystemEditor(SystemEditor_ptr&& se);
    Process& addConnection(Connection_ptr&& con);

    Value putToArgument(const Value& opInfo, const std::string& argName, const Value& value);

    Value putToArgumentViaConnection(const Value& conInfo, const Value& value);


    int32_t start();
    void startAsync();
    int32_t wait();
    void shutdown();
    
    //ProcessInfo info() const { return info_; }
    
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
    Value invokeConnection(const Connection& con);
    Value registerConsumerConnection(const ConnectionInfo& ci);
    Value registerProviderConnection(const ConnectionInfo& ci, BrokerAPI* receiverBroker=nullptr);
  
    Value deleteConsumerConnection(const ConnectionInfo& ci);
    Value deleteProviderConnection(const ConnectionInfo& ci);

  public:
    Value executeOperation(const Value& oinfo);

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
