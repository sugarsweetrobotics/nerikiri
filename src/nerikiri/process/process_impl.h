#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>

#include "nerikiri/nerikiri.h"

#include <nerikiri/process_api.h>
#include "nerikiri/corebroker.h"
#include "nerikiri/systemeditor.h"
#include "nerikiri/process_store.h"

#include "process_builder.h"

namespace nerikiri {
  
  /**
   * プロセスクラス
   */
  class NK_API Process : public ProcessAPI {
  private:
    static Process null;
   
    Value config_;

    std::shared_ptr<BrokerProxyAPI> coreBroker_;
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
    virtual ~Process();

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
    void _preStartFSMs();

    void _setupLogger();
  public:
    virtual ProcessStore* store() override { return &store_; }
    virtual const ProcessStore* store() const override { return &store_; }
    
    void setExecutablePath(const std::string& path) { path_ = path; }

    virtual std::shared_ptr<BrokerProxyAPI> coreBroker() override { return coreBroker_; }
    
    Value getCallbacks() const;

  public:

    Process& addSystemEditor(SystemEditor_ptr&& se);

    virtual Value fullInfo() const override;
    
    virtual int32_t start() override;

    virtual void startAsync() override;

    virtual int32_t wait() override;
    
    virtual void stop() override;
    
    virtual ProcessAPI& loadOperationFactory(const std::shared_ptr<OperationFactoryAPI>& opf) override {
      store()->addOperationFactory(opf); return *this;
    }

    virtual ProcessAPI& loadECFactory(const std::shared_ptr<ExecutionContextFactoryAPI>& ef) override {
      store()->addECFactory(ef); return *this;
    }

    
  public:
    virtual bool isRunning() override { return started_; }

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
