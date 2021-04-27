#pragma once
#include <vector>
#include <memory>

#include "nerikiri/nerikiri.h"
#include "nerikiri/dllproxy.h"
#include <nerikiri/operation_api.h>
#include <nerikiri/functional.h>
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
//#include "nerikiri/fsm_api.h"
//#include "nerikiri/fsm_factory_api.h"

namespace nerikiri {


  class Process;
  class ProcessAPI;
  class BrokerAPI;
  class BrokerFactoryAPI;

  class NK_API ProcessStore {
  private:
    ProcessAPI* process_;

    std::vector<std::shared_ptr<DLLProxy>> dllproxies_;

    std::vector<std::shared_ptr<Object>> objects_;

    std::vector<std::shared_ptr<OperationAPI>> operationProxies_;
    std::vector<std::shared_ptr<ExecutionContextFactoryAPI>> executionContextFactories_;
    
    std::vector<std::shared_ptr<BrokerAPI>> brokers_;
    std::vector<std::shared_ptr<BrokerFactoryAPI>> brokerFactories_;

    std::vector<std::shared_ptr<OperationInletAPI>> inletProxies_;
    std::vector<std::shared_ptr<OperationOutletAPI>> outletProxies_;

    friend class ProcessAPI;
    friend class Process;
  public:
    ProcessStore(ProcessAPI* process) : process_(process) {}

    ~ProcessStore() {
      /// クリアする順序が大事．他のオブジェクトへの直接のポインタを保持しているECなどは先に削除する必要がある

      executionContextFactories_.clear();
      brokers_.clear();
      brokerFactories_.clear();
    }

    /**
     * 基本情報取得
     * TODO: このクラスもObjectクラスを継承するべきだろうか
     */
    Value info() const ;

    ProcessAPI* process() { return process_; }


  private:

    template<typename T>
    std::vector<std::shared_ptr<T>>& ref_list();

  public:
    template<typename T>
    std::vector<std::shared_ptr<T>> list() const {
      std::vector<std::shared_ptr<T>> ops;// = {objects_.begin(), objects_.end()};
      for(auto& o : objects_) {
        auto op = std::dynamic_pointer_cast<T>(o);
        if (op) { ops.emplace_back(op); }
      }
      return ops;
    }

    template<>
    std::vector<std::shared_ptr<OperationAPI>> list() const { 
      std::vector<std::shared_ptr<OperationAPI>> ops;// = {objects_.begin(), objects_.end()};
      for(auto& o : objects_) {
        auto op = std::dynamic_pointer_cast<OperationAPI>(o);
        if (op) { ops.emplace_back(op); }
      }
      for(auto c : list<ContainerAPI>()) {
        auto cops = c->operations();
        std::copy(cops.begin(), cops.end(),std::back_inserter(ops));
      }
      return ops;
    }

  public:
    template<typename T>
    std::shared_ptr<T> get(const std::string& fullName) const {
      for(auto& e : list<T>()) { if (e->fullName() == fullName) return e; }
      return nullObject<T>();
    }

    template<typename T>
    Value add(const std::shared_ptr<T>& obj) {
      logger::trace("ProcessStore::add<{}>(objInfo={}) called",  obj->className(), obj->info());
      /// 同じ名前がないか確認
      if (!get<T>(obj->fullName())->isNull()) {
        return Value::error(logger::warn("ProcessStore.add<{}>(obj) failed. Object (fullName={} is already contained.", obj->className(), obj->fullName()));
      }
      if (obj->isNull()) {
        return Value::error(logger::warn("ProcessStore.add<{}>(obj) failed. Object (fullName={} is null.", obj->className(), obj->fullName()));
      }
      objects_.push_back(obj);
      //ref_list<T>().push_back(obj);
      logger::info("ProcessStore.add<{}>(obj->info()={}) succeeded.", obj->className(), obj->info());
      return obj->info();
    }

    template<typename T>
    Value del(const std::string& fullName) {
      /// 同じ名前がないか確認
      std::shared_ptr<T> c = get<T>(fullName);
      if (c->isNull()) {
        return Value::error(logger::warn("ProcessStore.del<>({}) failed. Object is not found.", fullName));
      }

      objects_.erase(std::remove_if(objects_.begin(), objects_.end(),
                              [&fullName](auto c){return c->fullName() == fullName; }), objects_.end());
      return c->info();
    }


  private:
    /**
     * 
     */
    template<class T>
    std::shared_ptr<T> getObject(std::vector<std::shared_ptr<T>>& collection, const std::string& fullName, std::function<std::shared_ptr<T>()> nullConstructor) {
      for(auto& c : collection) {
        if (c->info().at("fullName").stringValue() == fullName) return c;
      }
      return nullConstructor();
    }
    
    /**
     * 
     */
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
    Value addObject(std::vector<std::shared_ptr<T>>& collection, const std::shared_ptr<T>& obj, const std::string& ext) {
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
    Value delObject(std::vector<std::shared_ptr<T>>& collection, const std::string& fullName) {
      auto c = nerikiri::functional::find<std::shared_ptr<T>>(collection, [&fullName](auto c) { return c->fullName() == fullName; });
      if (!c) {
        return Value::error(logger::error("ProcessStore::delete<>({}) failed. Not found.", fullName));
      }
      collection.erase(std::remove_if(collection.begin(), collection.end(),
                              [&fullName](auto c){return c->fullName() == fullName; }), collection.end());
      return c.value()->info();
    } 

  public:
      
    std::vector<std::shared_ptr<OperationAPI>> operationProxies() const {
        return {operationProxies_.begin(), operationProxies_.end()};
    }

    std::vector<std::shared_ptr<OperationInletAPI>> inletProxies() const {
        return {inletProxies_.begin(), inletProxies_.end()};
    }

    std::vector<std::shared_ptr<OperationOutletAPI>> outletProxies() const {
        return {outletProxies_.begin(), outletProxies_.end()};
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
    
    Value addOperationProxy(const std::shared_ptr<OperationAPI>& operation) {
      return addObject<OperationAPI>(operationProxies_, operation, ".ope");
    }

    Value deleteOperationProxy(const std::string& fullName) {
      return delObject<OperationAPI>(operationProxies_, fullName);
    }

    Value addECFactory(const std::shared_ptr<ExecutionContextFactoryAPI>& ff) {
      return addObject<ExecutionContextFactoryAPI>(executionContextFactories_, ff, ".ecf");
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
    // std::shared_ptr<FSMAPI> fsmProxy(const Value& info);

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

    std::shared_ptr<ConnectionAPI> connection(const std::string& fullName) const;

    std::shared_ptr<ExecutionContextAPI> executionContext(const std::string& fullName) const;

    std::shared_ptr<ExecutionContextFactoryAPI> executionContextFactory(const std::string& ecTypeFullName) const;
    
    std::shared_ptr<BrokerAPI> broker(const std::string& fullName) const;

    std::shared_ptr<BrokerFactoryAPI> brokerFactory(const std::string& fullName) const;

    Value addDLLProxy(const std::shared_ptr<DLLProxy>& dllproxy);

    Value getCallbacks() const;
  };
}
