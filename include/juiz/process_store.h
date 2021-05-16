#pragma once
#include <vector>
#include <memory>

#include <juiz/juiz.h>
#include <juiz/utils/functional.h>
#include <juiz/utils/dllproxy.h>

#include <juiz/operation_api.h>
#include <juiz/container_api.h>

#include <juiz/ec_api.h>
#include <juiz/broker_api.h>

#include <juiz/connection.h>
#include <juiz/topic.h>

namespace juiz {



  class Process;
  class ProcessAPI;
  class BrokerAPI;
  class ClientProxyAPI;
  class BrokerFactoryAPI;

  std::shared_ptr<ClientProxyAPI> coreBroker(ProcessStore& store);

  std::shared_ptr<OperationAPI> operationProxy(const std::shared_ptr<ClientProxyAPI>& proxy, const std::string& fullName);

  class NK_API ProcessStore {
  private:
    ProcessAPI* process_;

    std::vector<std::shared_ptr<DLLProxy>> dllproxies_;

    std::vector<std::shared_ptr<Object>> objects_;

    std::vector<std::shared_ptr<OperationAPI>> operationProxies_;
    std::vector<std::shared_ptr<ContainerAPI>> containerProxies_;

    std::vector<std::shared_ptr<ExecutionContextFactoryAPI>> executionContextFactories_;
    
    std::vector<std::shared_ptr<BrokerAPI>> brokers_;
    std::vector<std::shared_ptr<BrokerFactoryAPI>> brokerFactories_;

    std::vector<std::shared_ptr<InletAPI>> inletProxies_;
    std::vector<std::shared_ptr<OutletAPI>> outletProxies_;

    std::vector<std::shared_ptr<ClientProxyAPI>> followerProxies_;

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
  public:
    bool addFollowerClientProxy(const std::shared_ptr<ClientProxyAPI>& followerProxy) {
      logger::trace("ProcessStore::addFollowerClientProxy() called");
      followerProxies_.push_back(followerProxy);
      return true;
    }

    bool removeFollowerClientProxy(const std::shared_ptr<ClientProxyAPI>& followerProxy) {
      logger::trace("ProcessStore::removeFollowerClientProxy() called");
      for (auto it = followerProxies_.begin(); it != followerProxies_.end();++it) {
        if (it->get() == followerProxy.get()) {
          it = followerProxies_.erase(it);
          return true;
        } 
      }
      return false;
    }

  private:

    template<typename T>
    std::vector<std::shared_ptr<T>>& ref_list();

  public:
    /**
     * 登録中のオブジェクトのリストを作成
     * @param T T型にdynamic_pointer_castできるオブジェクトのみをリスト化する
     * 
     */
    template<typename T>
    std::vector<std::shared_ptr<T>> list() const {
      std::vector<std::shared_ptr<T>> ops;// = {objects_.begin(), objects_.end()};
      for(auto& o : objects_) {
        auto op = std::dynamic_pointer_cast<T>(o);
        if (op) { ops.emplace_back(op); }
      }
      return ops;
    }

    template<typename T>
    std::vector<std::shared_ptr<T>> local_list() const {
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
    /**
     * 登録されているObjectをT型に変換して返す
     * 
     * fullNameは，/から開始した場合は絶対パス．
     * /から始まらない場合は，ローカルプロセスから検索を始めるので時間がかかる
     * 
     */
    template<typename T>
    std::shared_ptr<T> get(const std::string& fullName) const {
      /// 最初が / だったら
      if (fullName.length()==0) return nullObject<T>();
      if (fullName.at(0) == '/') {
        std::string fn = fullName.substr(1);
        /// まだ / を含んでいるなら，最初の要素はドメイン．
        if (fn.find('/') == std::string::npos) {
          auto tokens = juiz::stringSplit(fn, '/');
          if (tokens.size() != 2) { // / で分割して2つにならないということは，ドメインよりも多くの要素があって，現状は対応不可能
            return nullObject<T>();
          }
          /// /で区切って２つになるパターン．最初の要素がドメイン．二つ目がfullNameだ
          for(auto p : followerProxies_) {
            if (p->domain() == tokens[0]) {
              auto info = p->getProcessFullInfo();
            }
          }
        } else {
          /// / がなければローカルなObjectを指しているので返す
          for(auto& e : list<T>()) { if (e->fullName() == fullName) return e; }
          /// 見つからなければnullを返さなくてはならない．他のホストは検索しない
          return nullObject<T>();
        }
      } else { /// この場合はfullNameだけで他のホストも全力で検索して最初に見つかったやつを返す

        for(auto& e : list<T>()) { if (e->fullName() == fullName) return e; }
        for(auto& f : followerProxies_) {
          auto p = this->proxy<T>(fullName, f);
          if (!p->isNull()) {
            return p;
          }
        }
      }
      return nullObject<T>();
    }

    template<typename T>
    std::shared_ptr<T> proxy(const std::string& fullName, const std::shared_ptr<ClientProxyAPI>& clientProxy = nullptr) const {
      /// Proxyを作成するが，もし相手側が存在しなければnullObjectを返さなくてはならない　
      return nullObject<T>();
    }

    template<>
    std::shared_ptr<OperationAPI> proxy(const std::string& fullName, const std::shared_ptr<ClientProxyAPI>& clientProxy) const {
      /// Proxyを作成するが，もし相手側が存在しなければnullObjectを返さなくてはならない　
      auto op = juiz::operationProxy(clientProxy, fullName);
      if (op->fullInfo().isError()) return nullOperation();
      return op;
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
      logger::debug("ProcessStore.add<{}>(obj->info()={}) succeeded.", obj->className(), obj->info());
      return obj->info();
    }

    template<typename T>
    Value del(const std::string& fullName) {      
      logger::trace("ProcessStore::del<>(obj->fullName()={}) called", fullName);

      /// 同じ名前がないか確認
      std::shared_ptr<T> c = get<T>(fullName);
      if (c->isNull()) {
        return Value::error(logger::warn("ProcessStore.del<>({}) failed. Object is not found.", fullName));
      }
      logger::debug("ProcessStore.del<>(obj->fullName()={}) succeeded.", fullName);
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
      if (obj->isNull()) return Value::error(logger::error("Process::add({}) failed. Object is null.", juiz::demangle(typeid(T).name())));
      auto nameSpace = ""; /// ネームスペースは未来に実装予定

      if (obj->getInstanceName() == "") { /// infoへのinstanceName指定がまだなら自動指定する
        auto name = juiz::numbering_policy<std::shared_ptr<T>>(collection, obj->info().at("typeName").stringValue(), ext);
        obj->setFullName(nameSpace, name); /// fullName指定
      } else { /// instanceNameが指定されているなら，重複をチェックする
        for(auto& c : collection) {
          if (c->info().at("fullName") == obj->info().at("fullName")) {
            return Value::error(logger::error("ProcessStore::add({}) Error. Process already has the same name ({}) operation",  juiz::demangle(typeid(T).name()), obj->info().at("fullName")));
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
      if (obj->isNull()) { return Value::error(logger::error("ProcessStore::add<>({}) failed. Object is null.", juiz::demangle(typeid(T).name()))); }

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
      auto c = juiz::functional::find<std::shared_ptr<T>>(collection, [&fullName](auto c) { return c->fullName() == fullName; });
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

    std::vector<std::shared_ptr<InletAPI>> inletProxies() const {
        return {inletProxies_.begin(), inletProxies_.end()};
    }

    std::vector<std::shared_ptr<OutletAPI>> outletProxies() const {
        return {outletProxies_.begin(), outletProxies_.end()};
    }

    std::vector<std::shared_ptr<ContainerAPI>> containerProxies() const {
        return {containerProxies_.begin(), containerProxies_.end()};
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

    Value addContainerProxy(const std::shared_ptr<ContainerAPI>& o) {
      return addObject<ContainerAPI>(containerProxies_, o, ".ctn");
    }

    Value deleteContainerProxy(const std::string& fullName) {
      return delObject<ContainerAPI>(containerProxies_, fullName);
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
    Value addInletProxy(const std::shared_ptr<InletAPI>& inlet) {
      inletProxies_.push_back(inlet); //add<OperationInletAPI>(inletProxies_, inlet, "");
      return inlet->info();
    }

    Value addOutletProxy(const std::shared_ptr<OutletAPI>& outlet) {
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
    std::shared_ptr<InletAPI> inletProxy(const Value& info);

    /**
     * OutletProxyを取得する
     * 
     */
    std::shared_ptr<OutletAPI> outletProxy(const Value& info);

    std::shared_ptr<ContainerAPI> containerProxy(const Value& info);


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
