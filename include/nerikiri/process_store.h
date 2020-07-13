#pragma once
#include <vector>
#include "nerikiri/nerikiri.h"
#include "nerikiri/dllproxy.h"


#include "nerikiri/operation.h"
#include "nerikiri/container.h"
#include "nerikiri/ec.h"
#include "nerikiri/connection.h"
#include "nerikiri/connectiondictionary.h"
#include "nerikiri/operationfactory.h"

#include "nerikiri/broker.h"
#include "nerikiri/brokerfactory.h"
#include "nerikiri/topic.h"
#include "nerikiri/fsm.h"

namespace nerikiri {


  class Process;

  class NK_API ProcessStore {
  private:
    Process* process_;

    std::vector<std::shared_ptr<DLLProxy>> dllproxies_;

    std::vector<std::shared_ptr<OperationBase>> operations_;
    std::vector<std::shared_ptr<OperationFactory>> operationFactories_;

    std::vector<std::shared_ptr<ContainerBase>> containers_;
    std::vector<std::shared_ptr<ContainerFactoryBase>> containerFactories_;

    std::vector<std::shared_ptr<ExecutionContext>> executionContexts_;
    std::vector<std::shared_ptr<ExecutionContextFactory>> executionContextFactories_;
    
    std::vector<std::shared_ptr<Broker>> brokers_;
    std::vector<std::shared_ptr<BrokerFactory>> brokerFactories_;

    std::vector<std::shared_ptr<Topic>> topics_;
    std::vector<std::shared_ptr<TopicFactory>> topicFactories_;

    std::vector<std::shared_ptr<FSM>> fsms_;
    std::vector<std::shared_ptr<FSMFactory>> fsmFactories_;

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

  public:
    /**
     * Container全体にアクセス
     */
    std::vector<std::shared_ptr<ContainerBase>> getContainers() { return containers_; }


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
    Value add(std::vector<std::shared_ptr<T>>& collection, std::shared_ptr<T>&& obj, const std::string& ext) {
      logger::trace("Process::add({})", obj->info());
      /// まずはNULLオブジェクトならエラーを返す
      if (obj->isNull()) { return Value::error(logger::error("Process::add failed. Object is null.")); }

      auto info = updateFullName(collection, obj, ext);
      if (!info.isError()) {
         collection.emplace_back(std::move(obj));
      }
      return info;
    }
  public:
    Value getContainerInfos();
    Value getContainerFactoryInfos();

    
    /**
     * Containerの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addContainer(const std::shared_ptr<ContainerBase>& container);

    /**
     * Containerの取得
     */
    std::shared_ptr<ContainerBase> getContainer(const std::string& fullName);
    
    ProcessStore& addContainerFactory(const std::shared_ptr<ContainerFactoryBase> &cf);

    std::shared_ptr<ContainerFactoryBase> getContainerFactory(const Value& info);

    std::shared_ptr<OperationBase> getContainerOperation(const std::string& fullName);

    Value getOperationInfos();

    Value getAllOperationInfos();
    
    Value getOperationFactoryInfos();

    /**
     * Operationの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addOperation(const std::shared_ptr<Operation>& operation);

    /**
     * Operationの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addOperation(std::shared_ptr<Operation>&& operation);

    /**
     * Operationの取得
     */
    std::shared_ptr<OperationBase> getOperation(const std::string& fullName);

    std::shared_ptr<OperationBase> getAllOperation(const std::string& fullName);

    ProcessStore& addOperationFactory(const std::shared_ptr<OperationFactory>& opf);
    std::shared_ptr<OperationFactory> getOperationFactory(const Value& info);
    
    Value addConnection(Connection_ptr con);
    Value getConnectionInfos() const ;

    Value addContainerOperationFactory(const std::shared_ptr<ContainerOperationFactoryBase>& cof);

    /**
     * ExecutionContextの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addExecutionContext(const std::shared_ptr<ExecutionContext>& ec);

    /**
     * ExecutionContextの取得
     */
    std::shared_ptr<ExecutionContext> getExecutionContext(const std::string& fullName);

    ProcessStore& addExecutionContextFactory(const std::shared_ptr<ExecutionContextFactory>& ec);

    std::shared_ptr<ExecutionContextFactory> getExecutionContextFactory(const Value& info);

    Value getExecutionContextInfos();

    Value getExecutionContextFactoryInfos();
    

    /**
     * Brokerの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addBroker(const std::shared_ptr<Broker>& brk);

    /**
     * Brokerの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addBroker(std::shared_ptr<Broker>&& brk);

    /**
     * Brokerの取得
     */
    std::shared_ptr<Broker> getBroker(const std::string& fullName);

    Value addBrokerFactory(const std::shared_ptr<BrokerFactory>& factory);
    Value getBrokerInfos() const;
    std::shared_ptr<BrokerFactory> getBrokerFactory(const Value& info);

    Value addDLLProxy(const std::shared_ptr<DLLProxy>& dllproxy);

    Value getCallbacks() const;

    std::shared_ptr<TopicFactory> getTopicFactory(const Value& topicInfo);
    Value addTopicFactory(const std::shared_ptr<TopicFactory>& tf);
    std::shared_ptr<Topic> getTopic(const Value& topicInfo);
    Value getTopicInfos() const;
    Value addTopic(const std::shared_ptr<Topic>& topic);

    std::shared_ptr<OperationBase> getOperationOrTopic(const std::string& fullName);

    Value addFSM(const std::shared_ptr<FSM>& fsm);
    Value getFSMInfos() const;
    std::shared_ptr<FSMFactory> getFSMFactory(const Value& fsmInfo);
    Value addFSMFactory(const std::shared_ptr<FSMFactory>& ff);
    std::shared_ptr<FSM> getFSM(const std::string& fullName);

    Value deleteOperation(const std::string& fullName);
    Value deleteContainer(const std::string& fullName);
    Value deleteExecutionContext(const std::string& fullName);
    Value deleteFSM(const std::string& fullName);
  };
}