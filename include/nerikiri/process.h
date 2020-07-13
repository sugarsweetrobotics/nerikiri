#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>

#include "nerikiri/nerikiri.h"

#include "nerikiri/object.h"
#include "nerikiri/value.h"

#include "nerikiri/corebroker.h"

#include "nerikiri/systemeditor.h"
#include "nerikiri/process_store.h"

namespace nerikiri {
  
  /**
   * プロセスクラス
   */
  class NK_API Process : public Object {
  private:
    static Process null;
   
    Value config_;

    std::shared_ptr<CoreBroker> coreBroker_;
    ProcessStore store_;
   
    std::map<std::string, SystemEditor_ptr> systemEditors_;
    std::vector<std::shared_ptr<std::thread>> threads_;
    bool started_;
    std::string path_;
    std::function<void(Process*)> on_starting_;
    std::function<void(Process*)> on_started_;

    std::map<std::string, std::string> env_dictionary_;
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

    void parseConfigFile(const std::string& filepath);
  private:
    void _preloadOperations();
    void _preloadContainers();
    void _preloadExecutionContexts();
    void _preStartExecutionContexts();
    void _preloadBrokers();
    void _preloadConnections();
    void _preloadCallbacksOnStarted();
    void _preloadTopics();
    void _preloadFSMs();

    void _setupLogger();
  public:
    ProcessStore* store() { return &store_; }
    void setExecutablePath(const std::string& path) { path_ = path; }

    std::shared_ptr<CoreBroker> coreBroker() { return coreBroker_; }
    
    Value getCallbacks() const;

  public:

    Process& addSystemEditor(SystemEditor_ptr&& se);
    
    int32_t start();
    void startAsync();
    int32_t wait();
    void stop();
    

    Process& loadOperationFactory(const std::shared_ptr<OperationFactory>& opf) {
      store()->addOperationFactory(opf); return *this;
    }

    
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
  };

}
