#pragma once

#include "nerikiri/nerikiri.h"
#include "nerikiri/util/dllproxy.h"

#include "nerikiri/object.h"
#include "nerikiri/value.h"

#include "nerikiri/operation.h"
#include "nerikiri/brokers/brokerapi.h"
#include "nerikiri/brokers/broker.h"
#include "nerikiri/brokers/corebroker.h"

#include "nerikiri/systemeditor.h"
#include "nerikiri/containers/container.h"
#include "nerikiri/containers/containerfactory.h"
#include "nerikiri/containers/containeroperation.h"
#include "nerikiri/containers/containeroperationfactory.h"
#include "nerikiri/ec.h"
#include "nerikiri/process_store.h"
#include "nerikiri/objectfactory.h"
#include "nerikiri/operationfactory.h"
#include "nerikiri/objectmapper.h"


namespace nerikiri {

 //using ProcessInfo = Value;


  /**
   * プロセスクラス
   */
  class NK_API Process : public Object {
  private:
    static Process null;
   
    Value config_;
   
    std::map<std::string, SystemEditor_ptr> systemEditors_;
    std::vector<std::shared_ptr<std::thread>> threads_;
    ProcessStore store_;
    std::shared_ptr<CoreBroker> coreBroker_;
    bool started_;
    std::string path_;
    std::function<void(Process*)> on_starting_;
    std::function<void(Process*)> on_started_;
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
    void _preloadCallbacks();

    void _setupLogger();
  public:
    ProcessStore* store() { return &store_; }
    void setExecutablePath(const std::string& path) { path_ = path; }

    std::shared_ptr<CoreBroker> coreBroker() { return coreBroker_; }

  public:

    Process& addSystemEditor(SystemEditor_ptr&& se);
    
    int32_t start();
    void startAsync();
    int32_t wait();
    void stop();
    
  public:
    bool isRunning() { return started_; }

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

    void parseConfigFile(const std::string& filepath);
  };


  //  inline Process process(std::vector<std::string> args) { return std::move(Process(args.size() > 0 ? args[0] : "default_process_name")); }

}
