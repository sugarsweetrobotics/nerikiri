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
#include <nerikiri/ec_factory_api.h>
#include "nerikiri/broker_api.h"
#include "nerikiri/broker_factory_api.h"

#include "nerikiri/connection.h"
//#include "nerikiri/connectiondictionary.h"


#include "nerikiri/topic.h"
#include "nerikiri/fsm_api.h"
#include "nerikiri/fsm_factory_api.h"

namespace nerikiri {


  class Process;
  class BrokerAPI;
  class BrokerFactoryAPI;

  class NK_API ProcessStore {
  private:
    Process* process_;

    std::vector<std::shared_ptr<DLLProxy>> dllproxies_;

    std::vector<std::shared_ptr<OperationAPI>> operations_;
    std::vector<std::shared_ptr<OperationAPI>> operationProxies_;
    std::vector<std::shared_ptr<OperationFactoryAPI>> operationFactories_;

    std::vector<std::shared_ptr<ContainerAPI>> containers_;
    std::vector<std::shared_ptr<ContainerFactoryAPI>> containerFactories_;

    std::vector<std::shared_ptr<ContainerOperationFactoryAPI>> containerOperationFactories_;

    std::vector<std::shared_ptr<ExecutionContextAPI>> executionContexts_;
    std::vector<std::shared_ptr<ExecutionContextFactoryAPI>> executionContextFactories_;
    
    std::vector<std::shared_ptr<BrokerAPI>> brokers_;
    std::vector<std::shared_ptr<BrokerFactoryAPI>> brokerFactories_;

    std::vector<std::shared_ptr<TopicAPI>> topics_;
    std::vector<std::shared_ptr<TopicFactoryAPI>> topicFactories_;

    std::vector<std::shared_ptr<FSMAPI>> fsms_;
    std::vector<std::shared_ptr<FSMAPI>> fsmProxies_;
    std::vector<std::shared_ptr<FSMFactoryAPI>> fsmFactories_;

    std::vector<std::shared_ptr<OperationInletAPI>> inletProxies_;
    std::vector<std::shared_ptr<OperationOutletAPI>> outletProxies_;

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
      if (obj->isNull()) return Value::error(logger::error("Process::add({}) failed. Object is null.", nerikiri::demangle(typeid(T).name())));
      auto nameSpace = ""; /// ネームスペースは未来に実装予定

      if (obj->getInstanceName() == "") { /// infoへのinstanceName指定がまだなら自動指定する
        auto name = nerikiri::numbering_policy<std::shared_ptr<T>>(collection, obj->info().at("typeName").stringValue(), ext);
        obj->setFullName(nameSpace, name); /// fullName指定
      } else { /// instanceNameが指定されているなら，重複をチェックする
        for(auto& c : collection) {
          if (c->info().at("fullName") == obj->info().at("fullName")) {
            return Value::error(logger::error("ProcessStore::add({}) Error. Process already has the same name ({}) operation",  nerikiri::demangle(typeid(T).name()), obj->info().at("fullName")));
          }
        }
        //obj->setFullName(nameSpace, obj->getInstanceName()); /// fullName指定
      }
      return obj->info();
    }

    /**
     * オブジェクトの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    template<class T>
    Value add(std::vector<std::shared_ptr<T>>& collection, const std::shared_ptr<T>& obj, const std::string& ext) {
      /// まずはNULLオブジェクトならエラーを返す
      if (obj->isNull()) { return Value::error(logger::error("ProcessStore::add<>({}) failed. Object is null.", nerikiri::demangle(typeid(T).name()))); }

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
      auto c = nerikiri::functional::find<std::shared_ptr<T>>(collection, [&fullName](auto c) { return c->fullName() == fullName; });
      if (!c) {
        return Value::error(logger::error("ProcessStore::delete<>({}) failed. Not found.", fullName));
      }
      collection.erase(std::remove_if(collection.begin(), collection.end(),
                              [&fullName](auto c){return c->fullName() == fullName; }), collection.end());
      return c.value()->info();
    }

  public:
    /**
     * Get Operations (includes ContainerOperations)
     */
    std::vector<std::shared_ptr<OperationAPI>> operations() const {
      std::vector<std::shared_ptr<OperationAPI>> ops = {operations_.begin(), operations_.end()};
      for(auto c : containers()) {
        auto cops = c->operations();
        std::copy(cops.begin(), cops.end(),std::back_inserter(ops));
      }
      return ops;
    }
      
    std::vector<std::shared_ptr<OperationAPI>> operationProxies() const {
        return {operationProxies_.begin(), operationProxies_.end()};
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

    std::vector<std::shared_ptr<FSMAPI>> fsms() const;

    std::vector<std::shared_ptr<FSMAPI>> fsmProxies() const {
        return {fsmProxies_.begin(), fsmProxies_.end()};
    }

    std::vector<std::shared_ptr<OperationInletAPI>> inletProxies() const {
        return {inletProxies_.begin(), inletProxies_.end()};
    }

    std::vector<std::shared_ptr<OperationOutletAPI>> outletProxies() const {
        return {outletProxies_.begin(), outletProxies_.end()};
    }
    
    std::vector<std::shared_ptr<FSMFactoryAPI>> fsmFactories() const {
      return {fsmFactories_.begin(), fsmFactories_.end()};
    }

    std::vector<std::shared_ptr<TopicAPI>> topics() const {
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

    std::vector<std::shared_ptr<BrokerFactoryAPI>> brokerFactories() const {
      return {brokerFactories_.begin(), brokerFactories_.end()};
    }

    std::vector<std::shared_ptr<ConnectionAPI>> connections() const;
    /**
     * Operationの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addOperation(const std::shared_ptr<OperationAPI>& operation) {
      logger::trace("ProcessStore::addOperation({}) called", operation->info());
      return add<OperationAPI>(operations_, operation, ".ope");
    }

    Value addOperationProxy(const std::shared_ptr<OperationAPI>& operation) {
      return add<OperationAPI>(operationProxies_, operation, ".ope");
    }

    Value deleteOperation(const std::string& fullName) {
      return del<OperationAPI>(operations_, fullName);
    }

    Value deleteOperationProxy(const std::string& fullName) {
      return del<OperationAPI>(operationProxies_, fullName);
    }

    Value addOperationFactory(const std::shared_ptr<OperationFactoryAPI>& opf) {
      return add<OperationFactoryAPI>(operationFactories_, opf, ".opf");
    }

    Value deleteOperationFactory(const std::string& fullName) {
      return del<OperationFactoryAPI>(operationFactories_, fullName);
    }

    /**
     * Containerの追加．fullNameやinstanceNameの自動割り当ても行う
     */
    Value addContainer(const std::shared_ptr<ContainerAPI>& container, const std::string& ext = ".ctn") {
      return add<ContainerAPI>(containers_, container, ext);
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

    Value addFSMProxy(const std::shared_ptr<FSMAPI>& obj) {
      return add<FSMAPI>(fsmProxies_, obj, ".fsm");
    }

    Value deleteFSM(const std::string& fullName) {
      return del<FSMAPI>(fsms_, fullName);
    }

    Value deleteFSMProxy(const std::string& fullName) {
      return del<FSMAPI>(fsmProxies_, fullName);
    }

    Value addFSMFactory(const std::shared_ptr<FSMFactoryAPI>& ff) {
      return add<FSMFactoryAPI>(fsmFactories_, ff, ".ff");
    }

    /**
     * 
     */
    Value addTopic(const std::shared_ptr<TopicAPI>& t) {
      return add<TopicAPI>(topics_, t, ".topic");
    }

    Value addTopicFactory(const std::shared_ptr<TopicFactoryAPI>& f) {
      return add<TopicFactoryAPI>(topicFactories_, f, ".tf");
    }

    /**
     * 
     *
    Value addEC(const std::shared_ptr<ExecutionContextAPI>& ec) {
      return add<ExecutionContextAPI>(executionContexts_, ec, ".ec");
    } */
    /** 
    Value deleteEC(const std::string& fullName) {
      return del<ExecutionContextAPI>(executionContexts_, fullName);
    }
    */

    Value addECFactory(const std::shared_ptr<ExecutionContextFactoryAPI>& ff) {
      return add<ExecutionContextFactoryAPI>(executionContextFactories_, ff, ".ecf");
    }

    /**
     * 
     */
    Value addBroker(const std::shared_ptr<BrokerAPI>& b);
    Value deleteBroker(const std::string& fullName);

    Value addBrokerFactory(const std::shared_ptr<BrokerFactoryAPI>& bf);
    Value deleteBrokerFactory(const std::string& fullName);

    /**
     * InletProxyをStoreに追加
     */
    Value addInletProxy(const std::shared_ptr<OperationInletAPI>& inlet) {
      inletProxies_.push_back(inlet); //add<OperationInletAPI>(inletProxies_, inlet, "");
      return inlet->info();
    }

    Value addOutletProxy(const std::shared_ptr<OperationOutletAPI>& outlet) {
      outletProxies_.push_back(outlet);
      return outlet->info(); //return add<OperationOutletAPI>(outletProxies_, outlet, "");
    }

  public:
    // PROXIES 
    std::shared_ptr<OperationAPI> operationProxy(const Value& info);
    std::shared_ptr<FSMAPI> fsmProxy(const Value& info);

    /**
     * InletProxyを取得する
     */
    std::shared_ptr<OperationInletAPI> inletProxy(const Value& info);

    /**
     * OutletProxyを取得する
     * 
     */
    std::shared_ptr<OperationOutletAPI> outletProxy(const Value& info);


    //-------- getter ---------

    std::shared_ptr<OperationAPI> operation(const std::string& fullName) const;

    std::shared_ptr<ConnectionAPI> connection(const std::string& fullName) const;

    std::shared_ptr<OperationFactoryAPI> operationFactory(const std::string& operationTypeFullName) const;

    std::shared_ptr<ContainerAPI> container(const std::string& fullName) const;

    std::shared_ptr<ContainerFactoryAPI> containerFactory(const std::string& containerTypeFullName) const;

    std::shared_ptr<ContainerOperationFactoryAPI> containerOperationFactory(const std::string& containerOperationTypeFullName) const;

    std::shared_ptr<ContainerOperationFactoryAPI> containerOperationFactory(const std::string& containerTypeFullName, const std::string& operationTypeFullName) const;

    std::shared_ptr<FSMAPI> fsm(const std::string& fullName) const;

    std::shared_ptr<FSMFactoryAPI> fsmFactory(const std::string& fsmTypeFullName) const;

    std::shared_ptr<TopicAPI> topic(const std::string& fullName) const;

    std::shared_ptr<TopicFactoryAPI> topicFactory(const std::string& topicTypeFullName) const;

    std::shared_ptr<ExecutionContextAPI> executionContext(const std::string& fullName) const;

    std::shared_ptr<ExecutionContextFactoryAPI> executionContextFactory(const std::string& ecTypeFullName) const;
    
    std::shared_ptr<BrokerAPI> broker(const std::string& fullName) const;

    std::shared_ptr<BrokerFactoryAPI> brokerFactory(const std::string& fullName) const;

    Value addDLLProxy(const std::shared_ptr<DLLProxy>& dllproxy);

    Value getCallbacks() const;

    std::shared_ptr<OperationAPI> getOperationOrTopic(const std::string& fullName);


  };
}
