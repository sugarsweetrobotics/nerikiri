#pragma once
#include <vector>
#include "nerikiri/nerikiri.h"
#include "nerikiri/dllproxy.h"


#include <nerikiri/operation_api.h>
#include <nerikiri/operation_factory_api.h>
#include <nerikiri/container_api.h>
#include <nerikiri/container_factory_api.h>
#include <nerikiri/container_operation_factory_api.h>
#include <nerikiri/ec_api.h>
#include "nerikiri/connection.h"
#include "nerikiri/connectiondictionary.h"

#include "nerikiri/broker_api.h"
#include "nerikiri/broker_factory_api.h"

#include "nerikiri/topic.h"
#include "nerikiri/fsm.h"

namespace nerikiri {


  class Process;

  class NK_API ProcessStore {
  private:
    Process* process_;

    std::vector<std::shared_ptr<DLLProxy>> dllproxies_;

    std::vector<std::shared_ptr<OperationAPI>> operations_;
    std::vector<std::shared_ptr<OperationFactoryAPI>> operationFactories_;

    std::vector<std::shared_ptr<ContainerAPI>> containers_;
    std::vector<std::shared_ptr<ContainerFactoryAPI>> containerFactories_;


    std::vector<std::shared_ptr<ContainerOperationFactoryAPI>> containerOperationFactories_;

    std::vector<std::shared_ptr<ExecutionContextAPI>> executionContexts_;
    std::vector<std::shared_ptr<ExecutionContextFactoryAPI>> executionContextFactories_;
    
    std::vector<std::shared_ptr<BrokerAPI>> brokers_;
    std::vector<std::shared_ptr<BrokerFactoryAPI>> brokerFactories_;

    std::vector<std::shared_ptr<TopicBase>> topics_;
    std::vector<std::shared_ptr<TopicFactoryAPI>> topicFactories_;

    std::vector<std::shared_ptr<FSMAPI>> fsms_;
    std::vector<std::shared_ptr<FSMFactoryAPI>> fsmFactories_;

    friend class Process;
  public:
    ProcessStore(Process* process) : process_(process) {}

    ~ProcessStore() {
      /// クリアする順序が大事．他のオブジェクトへの直接のポインタを保持しているECなどは先に削除する必要がある
      fsms_.clear();

      topics_.clear();
      topicFactories_.clear();
      executionContexts_.clear();
      executionContextFactories_.clear();
      operations_.clear();
      operationFactories_.clear();
      containers_.clear();
      containerFactories_.clear();
      brokers_.clear();
      brokerFactories_.clear();
    }

    /**
     * 基本情報取得
     * TODO: このクラスもObjectクラスを継承するべきだろうか
     */
    Value info() const ;



  private:

    /**
     * 
     */
    template<class T>
    std::shared_ptr<T> get(std::vector<std::shared_ptr<T>>& collection, const std::string& fullName, std::function<std::shared_ptr<T>()> nullConstructor) {
      for(auto& c : collection) {
        if (c->info().at("fullName").stringValue() == fullName) return c;
      }
      return nullConstructor();
    }

    template<class T>
    Value updateFullName(std::vector<std::shared_ptr<T>>& collection, const std::shared_ptr<T>& obj, const std::string& ext) {
      if (obj->isNull()) return Value::error(logger::error("Process::add failed. Object is null."));
      auto nameSpace = ""; /// ネームスペースは未来に実装予定
      if (obj->getInstanceName() == "") { /// infoへのinstanceName指定がまだなら自動指定する
        auto name = nerikiri::numbering_policy<std::shared_ptr<T>>(collection, obj->info().at("typeName").stringValue(), ext);
        obj->setFullName(nameSpace, name); /// fullName指定
      } else { /// instanceNameが指定されているなら，重複をチェックする
        for(auto& c : collection) {
          if (c->info().at("instanceName") == obj->info().at("instanceName")) {
            return Value::error(logger::error("ProcessStore::add({}) Error. Process already has the same name operation", typeid(T).name()));
          }
        }
        obj->setFullName(nameSpace, obj->getInstanceName()); /// fullName指定
      }
      return obj->info();
    }

    /**
     * オブジェクトの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    template<class T>
    Value add(std::vector<std::shared_ptr<T>>& collection, const std::shared_ptr<T>& obj, const std::string& ext) {
      logger::trace("Process::add({})", obj->info());
      /// まずはNULLオブジェクトならエラーを返す
      if (obj->isNull()) { return Value::error(logger::error("ProcessStore::add{} failed. Object is null.", typeid(T).name())); }

      auto info = updateFullName(collection, obj, ext);
      if (!info.isError()) {
         collection.push_back(obj);
      }
      return info;
    }
        
    /**
     * オブジェクトの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    template<class T>
    Value del(std::vector<std::shared_ptr<T>>& collection, const std::string& fullName) {
      logger::trace("Process::del({})", fullName);
      auto c = nerikiri::functional::find<std::shared_ptr<T>>(collection, [&fullName](auto c) { return c->fullName() == fullName; });
      if (!c) {
        return Value::error(logger::error("ProcessStore::delete<>({}) failed. Not found.", fullName));
      }
      collection.erase(std::remove_if(collection.begin(), collection.end(),
                              [&fullName](auto c){return c->fullName() == fullName; }), collection.end());
      return c.value()->info();
    }
  public:
    // PROXIES 
    std::shared_ptr<OperationAPI> operationProxy(const Value& info);

  public:
    /**
     * Get Operations (includes ContainerOperations)
     */
    std::vector<std::shared_ptr<OperationAPI>> operations() const {
      std::vector<std::shared_ptr<OperationAPI>> ops(operations_.begin(), operations_.end());
      /*
      for(auto& c : containers_) { 
        auto cops = c->operations();
        std::copy(cops.begin(), cops.end(), std::back_inserter(ops));
      }
      */
      return ops;
    }

    std::vector<std::shared_ptr<OperationFactoryAPI>> operationFactories() const {
      return {operationFactories_.begin(), operationFactories_.end()};
    }

    std::vector<std::shared_ptr<ContainerAPI>> containers() const {
      return {containers_.begin(), containers_.end()};
    }

    std::vector<std::shared_ptr<ContainerFactoryAPI>> containerFactories() const {
      return {containerFactories_.begin(), containerFactories_.end()};
    }

    std::vector<std::shared_ptr<ContainerOperationFactoryAPI>> containerOperationFactories() const {
      return {containerOperationFactories_.begin(), containerOperationFactories_.end()};
    }

    std::vector<std::shared_ptr<FSMAPI>> fsms() const {
      return {fsms_.begin(), fsms_.end()};
    }

    std::vector<std::shared_ptr<FSMFactoryAPI>> fsmFactories() const {
      return {fsmFactories_.begin(), fsmFactories_.end()};
    }

    std::vector<std::shared_ptr<TopicBase>> topics() const {
      return {topics_.begin(), topics_.end()};
    }

    std::vector<std::shared_ptr<TopicFactoryAPI>> topicFactories() const {
      return {topicFactories_.begin(), topicFactories_.end()};
    }

    std::vector<std::shared_ptr<ExecutionContextAPI>> executionContexts() const { 
      return {executionContexts_.begin(), executionContexts_.end()};
    }
    
    std::vector<std::shared_ptr<ExecutionContextFactoryAPI>> executionContextFactories() const { 
      return {executionContextFactories_.begin(), executionContextFactories_.end()};
    }

    std::vector<std::shared_ptr<BrokerAPI>> brokers() const { 
      return {brokers_.begin(), brokers_.end()};
    }

    std::vector<std::shared_ptr<BrokerFactoryAPI> brokerFactories() const {
      return {brokerFactories_.begin(), brokerFactories_.end()};
    }

    /**
     * Operationの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addOperation(const std::shared_ptr<OperationAPI>& operation) {
      return add<OperationAPI>(operations_, operation, ".ope");
    }


    Value deleteOperation(const std::string& fullName) {
      return del<OperationAPI>(operations_, fullName);
    }

    Value addOperationFactory(const std::shared_ptr<OperationFactory>& opf) {
      return add<OperationFactoryAPI>(operationFactories_, opf, ".opf");
    }

    /**
     * Containerの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addContainer(const std::shared_ptr<ContainerAPI>& container) {
      return add<ContainerAPI>(containers_, container, ".ctn");
    }

    Value deleteContainer(const std::string& fullName) {
      return del<ContainerAPI>(containers_, fullName);
    }

    Value addContainerFactory(const std::shared_ptr<ContainerFactoryAPI> &cf) {
      return add<ContainerFactoryAPI>(containerFactories_, cf, ".cf");
    }

    Value addContainerOperationFactory(const std::shared_ptr<ContainerOperationFactoryAPI> &cf) {
      return add<ContainerOperationFactoryAPI>(containerOperationFactories_, cf, ".cof");
    }

    Value deleteContainerOperation(const std::string& fullName) {
      return del<OperationAPI>(operations_, fullName);
    }


    /**
     * 
     */
    Value addFSM(const std::shared_ptr<FSMAPI>& fsm) {
      return add<FSMAPI>(fsms_, fsm, ".fsm");
    }

    Value deleteFSM(const std::string& fullName) {
      return del<FSMAPI>(fsms_, fullName);
    }

    Value addFSMFactory(const std::shared_ptr<FSMFactoryAPI>& ff) {
      return add<FSMFactoryAPI>(fsmFactories_, ff, ".ff");
    }

    /**
     * 
     */
    Value addTopic(const std::shared_ptr<TopicBase>& t) {
      return add<TopicBase>(topics_, t, ".topic");
    }

    Value addTopicFactory(const std::shared_ptr<TopicFactoryAPI>& f) {
      return add<TopicFactoryAPI>(topicFactories_, f, ".tf");
    }

    /**
     * 
     */
    Value addEC(const std::shared_ptr<ExecutionContextAPI>& ec) {
      return add<ExecutionContextAPI>(executionContexts_, ec, ".ec");
    }

    Value deleteEC(const std::string& fullName) {
      return del<ExecutionContextAPI>(executionContexts_, fullName);
    }

    Value addECFactory(const std::shared_ptr<ExecutionContextFactoryAPI>& ff) {
      return add<ExecutionContextFactoryAPI>(executionContextFactories_, ff, ".ecf");
    }

    /**
     * 
     */
    Value addBroker(const std::shared_ptr<BrokerAPI>& b) {
      return add<BrokerAPI>(brokers_, b, ".brk");
    }

    Value deleteBroker(const std::string& fullName) {
      return del<BrokerAPI>(brokers_, fullName);
    }

    Value addBrokerFactory(const std::shared_ptr<BrokerFactoryAPI>& bf) {
      return add<BrokerFactoryAPI>(brokerFactories_, bf, ".bf");
    }

    //-------- getter ---------

    std::shared_ptr<OperationAPI> operation(const std::string& fullName) const { 
      auto op = nerikiri::functional::find<std::shared_ptr<OperationAPI>>(operations(), [&fullName](auto op) { return op->fullName() == fullName; });
      if (op) return op.value();;
      return std::make_shared<NullOperation>();
    }

    std::shared_ptr<OperationFactoryAPI> operationFactory(const std::string& operationTypeFullName) {
      auto f = nerikiri::functional::find<std::shared_ptr<OperationFactoryAPI>>(operationFactories(), [&operationTypeFullName](auto f) { return f->operationTypeFullName() == operationTypeFullName; });
      if (f) return f.value();
      return std::make_shared<NullOperationFactory>();
    }

    std::shared_ptr<ContainerAPI> container(const std::string& fullName) const { 
      auto op = nerikiri::functional::find<std::shared_ptr<ContainerAPI>>(containers(), [&fullName](auto op) { return op->fullName() == fullName; });
      if (op) return op.value();;
      return std::make_shared<NullContainer>();
    }

    std::shared_ptr<ContainerFactoryAPI> containerFactory(const std::string& containerTypeFullName) const {
      auto f = nerikiri::functional::find<std::shared_ptr<ContainerFactoryAPI>>(containerFactories(), [&containerTypeFullName] (auto f) {
        return f->containerTypeFullName() == containerTypeFullName;
      });
      if (f) return f.value();
      return std::make_shared<NullContainerFactory>();
    }

    std::shared_ptr<ContainerOperationFactoryAPI> containerOperationFactory(const std::string& containerOperationTypeFullName) const {
      auto& [containerTypeFullName, operationTypeFullName] = nerikiri::naming::splitContainerAndOperationName(containerOperationTypeFullName);
      return containerOperationFactory(containerTypeFullName, operationTypeFullName);
    }

    std::shared_ptr<ContainerOperationFactoryAPI> containerOperationFactory(const std::string& containerTypeFullName, const std::string& operationTypeFullName) const {
      auto f = nerikiri::functional::find<std::shared_ptr<ContainerOperationFactoryAPI>>(containerOperationFactories(), [&containerTypeFullName, &operationTypeFullName] (auto f) {
        return f->containerTypeFullName() == containerTypeFullName && f->operationTypeFullName() == operationTypeFullName;
      });
      if (f) return f.value();
      return std::make_shared<NullContainerOperationFactory>();
    }

    std::shared_ptr<FSMAPI> fsm(const std::string& fullName) const {
      auto f = nerikiri::functional::find<std::shared_ptr<FSMAPI>>(fsms(), [&fullName](auto fsm) { return fsm->fullName() == fullName; });
      if (f) return f.value();;
      return std::make_shared<NullFSM>();
    }

    std::shared_ptr<FSMFactoryAPI> fsmFactory(const std::string& fsmTypeFullName) const {
      auto f = nerikiri::functional::find<std::shared_ptr<FSMFactoryAPI>>(fsmFactories(), [&fsmTypeFullName] (auto f) {
        return f->fsmTypeFullName() == fsmTypeFullName;
      });
      if (f) return f.value();
      return std::make_shared<NullFSMFactory>();
    }

    std::shared_ptr<TopicBase> topic(const std::string& fullName) const {
      auto f = nerikiri::functional::find<std::shared_ptr<TopicBase>>(topics(), [&fullName](auto t) { return t->fullName() == fullName; });
      if (f) return f.value();;
      return std::make_shared<NullTopic>();
    }

    std::shared_ptr<TopicFactoryAPI> topicFactory(const std::string& topicTypeFullName) const {
      auto f = nerikiri::functional::find<std::shared_ptr<TopicFactoryAPI>>(topicFactories(), [&topicTypeFullName] (auto f) {
        return f->topicTypeFullName() == topicTypeFullName;
      });
      if (f) return f.value();
      return std::make_shared<NullTopicFactory>();
    }

    std::shared_ptr<ExecutionContextAPI> executionContext(const std::string& fullName) const {
      auto f = nerikiri::functional::find<std::shared_ptr<ExecutionContextAPI>>(executionContexts(), [&fullName](auto ec) { return ec->fullName() == fullName; });
      if (f) return f.value();;
      return std::make_shared<NullExecutionContext>();
    }

    std::shared_ptr<ExecutionContextFactoryAPI> executionContextFactory(const std::string& ecTypeFullName) const {
      auto f = nerikiri::functional::find<std::shared_ptr<ExecutionContextFactoryAPI>>(executionContextFactories(), [&ecTypeFullName] (auto f) {
        return f->executionContextTypeFullName() == ecTypeFullName;
      });
      if (f) return f.value();
      return std::make_shared<NullExecutionContextFactory>();
    }
    
    std::shared_ptr<BrokerAPI> broker(const std::string& fullName) const {
      auto f = nerikiri::functional::find<std::shared_ptr<BrokerAPI>>(brokers(), [&fullName](auto b) { return b->fullName() == fullName; });
      if (f) return f.value();;
      return std::make_shared<NullBroker>();
    }

    std::shared_ptr<BrokerFactoryAPI> brokerFactory(const std::string& fullName) const {
      auto f = nerikiri::functional::find<std::shared_ptr<BrokerFactoryAPI>>(brokerFactories(), [&fullName](auto f) { return f->fullName() == fullName; });
      if (f) return f.value();;
      return std::make_shared<NullBrokerFactory>();
    }

    Value addDLLProxy(const std::shared_ptr<DLLProxy>& dllproxy);

    Value getCallbacks() const;

    std::shared_ptr<OperationBase> getOperationOrTopic(const std::string& fullName);


  };
}