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

//#include "nerikiri/brokers/corebroker.h"
#include "nerikiri/brokerfactory.h"
#include "nerikiri/topic.h"

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
    friend class Process;
  public:
    ProcessStore(Process* process) : process_(process) {}

    ~ProcessStore() {
      /// クリアする順序が大事．他のオブジェクトへの直接のポインタを保持しているECなどは先に削除する必要がある
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
    std::shared_ptr<T> get(std::vector<std::shared_ptr<T>>& collection, const Value& info, std::function<std::shared_ptr<T>()> nullConstructor) {
      for(auto& c : collection) {
        if (c->info().at("fullName") == info.at("fullName")) return c;
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
            return Value::error(logger::error("Process::add({}) Error. Process already has the same name operation", obj->info().at("name").stringValue()));
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
      if (obj->isNull()) { return Value::error(logger::error("Process::add failed. Object is null.")); }

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
    Value addContainer(const std::shared_ptr<ContainerBase>& container) {
      return add<ContainerBase>(containers_, container, ".ctn");
    }

    /**
     * Containerの取得
     */
    std::shared_ptr<ContainerBase> getContainer(const Value& info) {
      return get<ContainerBase>(containers_, info, nullContainer);
    }
    
    ProcessStore& addContainerFactory(std::shared_ptr<ContainerFactoryBase> cf);
    std::shared_ptr<ContainerFactoryBase> getContainerFactory(const Value& info);
    std::shared_ptr<OperationBase> getContainerOperation(const Value& oi);

    Value getOperationInfos();

    Value getAllOperationInfos();
    
    Value getOperationFactoryInfos();

    /**
     * Operationの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addOperation(const std::shared_ptr<Operation>& operation) {
      auto temp = std::dynamic_pointer_cast<OperationBase>(operation);
      return add<OperationBase>(operations_, temp, ".ope");
    }

    /**
     * Operationの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addOperation(std::shared_ptr<Operation>&& operation) {
      return add<OperationBase>(operations_, std::dynamic_pointer_cast<OperationBase>(operation), ".ope");
    }

    /**
     * Operationの取得
     */
    std::shared_ptr<OperationBase> getOperation(const Value& info) {
      return get<OperationBase>(operations_, info, nullOperation);
    }

    std::shared_ptr<OperationBase> getAllOperation(const Value& info) {
      auto op = getOperation(info);
      if (op->isNull()) {
        return getContainerOperation(info);
      }
      return op;
    }

    ProcessStore& addOperationFactory(std::shared_ptr<OperationFactory> opf);
    std::shared_ptr<OperationFactory> getOperationFactory(const Value& info);
    
    Value addConnection(Connection_ptr con);
    Value getConnectionInfos() const ;

    Value addContainerOperationFactory(std::shared_ptr<ContainerOperationFactoryBase> cof);

    /**
     * ExecutionContextの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addExecutionContext(const std::shared_ptr<ExecutionContext>& ec) {
      return add<ExecutionContext>(executionContexts_, ec, ".ec");
    }

    /**
     * ExecutionContextの取得
     */
    std::shared_ptr<ExecutionContext> getExecutionContext(const Value& info) {
      return get<ExecutionContext>(executionContexts_, info, nullExecutionContext);
    }

    ProcessStore& addExecutionContextFactory(std::shared_ptr<ExecutionContextFactory> ec);
    std::shared_ptr<ExecutionContextFactory> getExecutionContextFactory(const Value& info);
    Value getExecutionContextInfos();
    Value getExecutionContextFactoryInfos();
    

    /**
     * Brokerの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addBroker(const std::shared_ptr<Broker>& brk) {
      return add<Broker>(brokers_, brk, ".brk");
    }

    /**
     * Brokerの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addBroker(std::shared_ptr<Broker>&& brk) {
      return add<Broker>(brokers_, std::move(brk), ".brk");
    }

    /**
     * Brokerの取得
     */
    std::shared_ptr<Broker> getBroker(const Value& info) {
      return get<Broker>(brokers_, info, nullBroker);
    }

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


    Value deleteOperation(const Value& info);
    Value deleteContainer(const Value& info);
    Value deleteExecutionContext(const Value& info);
  };
}