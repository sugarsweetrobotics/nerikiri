#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>

#include <juiz/nerikiri.h>

#include <juiz/process_api.h>
#include "../broker/core_broker.h"
#include "juiz/systemeditor.h"
#include <juiz/process_store.h>

#include "process_builder.h"

namespace juiz {

  class ProcessProxy : public ProcessAPI {
  private:
    const std::shared_ptr<ClientProxyAPI> proxy_;
    const std::string name_;

  public:
    ProcessProxy(const std::string& name, const std::shared_ptr<ClientProxyAPI>& proxy)
     : ProcessAPI("Process", "Proxy", name), name_(name), proxy_(proxy) {}
    virtual ~ProcessProxy() {}
    

        virtual int32_t start() {

        }

        virtual void startAsync() {

        }

        virtual int32_t wait() {

        }

        virtual Value fullInfo() const {

        }

        virtual void stop() {
          
        }

        virtual bool isRunning() {
          
        }
        // virtual std::shared_ptr<BrokerProxyAPI> coreBroker() = 0;

        virtual ProcessStore* store() {

        }

        virtual const ProcessStore* store() const {

        }

        virtual ProcessAPI& load(const std::shared_ptr<FactoryAPI>& f) {

        }

        virtual ProcessAPI& loadECFactory(const std::shared_ptr<ExecutionContextFactoryAPI>& ef) {

        }

        virtual ProcessAPI& conductProcess(const Value& brokerInfo) {

        }

  };

  inline std::shared_ptr<ProcessAPI> processProxy(const std::string& name, const std::shared_ptr<ClientProxyAPI>& clientProxy) {
    return std::make_shared<ProcessProxy>(name, clientProxy);
  }

  
  /**
   * プロセスクラス
   */
  class NK_API Process : public ProcessAPI {
  private:
    static Process null;
   
    Value config_;

    std::shared_ptr<ClientProxyAPI> coreBroker_;
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

    // virtual std::shared_ptr<BrokerProxyAPI> coreBroker() override { return coreBroker_; }
    
    Value getCallbacks() const;

  public:

    Process& addSystemEditor(SystemEditor_ptr&& se);

    virtual Value fullInfo() const override;
    
    virtual int32_t start() override;

    virtual void startAsync() override;

    virtual int32_t wait() override;
    
    virtual void stop() override;

    virtual ProcessAPI& load(const std::shared_ptr<FactoryAPI>& f) override {
      store()->add<FactoryAPI>(f); return *this;
      return *this;
    }

    /*
    virtual ProcessAPI& loadOperationFactory(const std::shared_ptr<OperationFactoryAPI>& opf) override {
      store()->add<OperationFactoryAPI>(opf); return *this;
    }

    virtual ProcessAPI& loadContainerFactory(const std::shared_ptr<ContainerFactoryAPI>& cf) override {
      store()->add<ContainerFactoryAPI>(cf); return *this;
    }

    virtual ProcessAPI& loadContainerOperationFactory(const std::shared_ptr<ContainerOperationFactoryAPI>& copf) override {
      store()->add<ContainerOperationFactoryAPI>(copf); return *this;
    }
    */

    virtual ProcessAPI& loadECFactory(const std::shared_ptr<ExecutionContextFactoryAPI>& ef) override {
      store()->addECFactory(ef); return *this;
    }

  private:
  
  public:
    virtual ProcessAPI& conductProcess(const Value& brokerInfo) override {
      logger::trace("ProcessImpl::conductProcess({}) called", brokerInfo);
      int count = 0;
      std::string fullName = "";
      do {
        fullName = Value::string(brokerInfo["fullName"], Value::string(brokerInfo["typeName"])+std::to_string(count++));
      } while (store()->broker(fullName)->isNull());
      logger::trace("ProcessImpl::conductProcess() exit.");
      store()->addFollowerClientProxy(store()->brokerFactory(Value::string(brokerInfo["typeName"]))->createProxy(brokerInfo));
      // return addFollowerProcess(processProxy(fullName, store()->brokerFactory(Value::string(brokerInfo["typeName"]))->createProxy(brokerInfo)));
      return *this;
    }

    virtual ProcessAPI& addFollowerProcess(const std::shared_ptr<ProcessAPI>& proc) {
      if (proc->isNull()) {
        logger::error("ProcessImpl::addFollowerProcess() failed. proc is null");
        return *this;
      }



      return *this;
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
