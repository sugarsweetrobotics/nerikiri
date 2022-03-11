#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <filesystem>

#include <juiz/juiz.h>

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
          return 0;
        }

        virtual void startAsync() {

        }

        virtual int32_t wait() {
          return 0;
        }

        virtual Value fullInfo() const {
          return Object::fullInfo();
        }

        virtual void stop() {
          
        }

        virtual bool isRunning() {
          return false;
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
  class NK_API ProcessImpl : public ProcessAPI {
  private:
   
    Value config_;

    std::shared_ptr<ClientProxyAPI> coreBroker_;

    ProcessStore store_;
   
    std::map<std::string, SystemEditor_ptr> systemEditors_;
    std::vector<std::shared_ptr<std::thread>> threads_;
    bool started_;
    
    std::filesystem::path fullpath_;

    std::function<void(ProcessAPI*)> on_starting_;
    std::function<void(ProcessAPI*)> on_started_;

    std::map<std::string, std::string> env_dictionary_;
  public:
    /**
     * コンストラクタ
     * @param name
     */
    ProcessImpl(const std::string& name);

    ProcessImpl(const int argc, const char** argv);

    ProcessImpl(const std::string& name, const Value& config);

    ProcessImpl(const std::string& name, const std::string& jsonStr);

    /**
     * デストラクタ
     */
    virtual ~ProcessImpl();

  private:

    void _setupLogger(const Value& loggerValue) const;

    std::filesystem::path _guessFullPath(const std::string& commandName) const;

    void _parseConfigFile(const std::filesystem::path& filepath);
    
  public:

    virtual ProcessStore* store() override { return &store_; }

    virtual const ProcessStore* store() const override { return &store_; }
    
    Value getCallbacks() const;

  public:

    ProcessImpl& addSystemEditor(SystemEditor_ptr&& se);

    virtual Value fullInfo() const override;
    
    virtual int32_t start() override;

    virtual void startAsync() override;

    virtual int32_t wait() override;
    
    virtual void stop() override;

    virtual ProcessAPI& load(const std::shared_ptr<FactoryAPI>& f) override {
      store()->add<FactoryAPI>(f); return *this;
      return *this;
    }

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
    ProcessAPI& setOnStarting(std::function<void(ProcessAPI*)> f) {
      on_starting_ = f;
      return *this;
    }
    ProcessAPI& setOnStarted(std::function<void(ProcessAPI*)> f) {
      on_started_ = f;
      return *this;
    }
  };

}
