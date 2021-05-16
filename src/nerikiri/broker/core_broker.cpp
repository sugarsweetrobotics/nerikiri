// #include <utility>

#include <juiz/process_api.h>
#include <juiz/operation.h>
#include <juiz/connection.h>
#include <juiz/container.h>
#include "core_broker.h"
#include "core_broker_impl.h"
#include "../proxy_builder.h"
#include "../objectfactory.h"
#include "../connection/connection_builder.h"

using namespace juiz;


class CoreBrokerFactory : public juiz::BrokerFactoryAPI {
private:
  std::shared_ptr<CoreBroker> coreBroker_;

public:
  CoreBrokerFactory(const std::shared_ptr<CoreBroker>& coreBroker)
  : BrokerFactoryAPI("CoreBrokerFactory", "CoreBroker", "coreBrokerFactory"), coreBroker_(coreBroker) {}
  virtual ~CoreBrokerFactory() {}

public:
  virtual std::shared_ptr<BrokerAPI> create(const Value& param) override { 
    return nullBroker();
  }

  virtual std::shared_ptr<ClientProxyAPI> createProxy(const Value& param) override {
      return coreBroker_;
  }
};


std::shared_ptr<BrokerFactoryAPI> juiz::coreBrokerFactory(ProcessAPI* process, const std::string& fullName) {
    return std::make_shared<CoreBrokerFactory>(std::make_shared<CoreBroker>(process, fullName));
}

class CoreFactoryBroker : public FactoryClientAPI {
private:
  ProcessAPI* process_;
  ClientProxyAPI* coreBroker_;
public:
  CoreFactoryBroker(ProcessAPI* proc, ClientProxyAPI* coreBroker) : process_(proc), coreBroker_(coreBroker) {}
  virtual ~CoreFactoryBroker() {}

  virtual Value createObject(const std::string& className, const Value& info={}) override {
        logger::trace("CoreBroker::CoreFactoryBroker::createObject({}, {})", className, info);
        if (className == "operation") {
            return ObjectFactory::createOperation(*process_->store(), info);
        } else if (className == "container") {
            return ObjectFactory::createContainer(*process_->store(), info);
        } else if (className == "containerOperation") {
            return ObjectFactory::createContainerOperation(*process_->store(), process_->store()->get<ContainerAPI>(Value::string(info.at("containerFullName")))->info(), info);
        } else if (className == "ec") {
            return ObjectFactory::createExecutionContext(*process_->store(), info);
        } else if (className == "fsm") {
            return ObjectFactory::createFSM(*process_->store(), info);
        } else if (className == "topic") {
            return ObjectFactory::createTopic(*process_->store(), info);
        } else if (className == "connection") {
            return ConnectionBuilder::createOperationConnection(*process_->store(), info);
        } else if (className == "outletConnection") {
            return ConnectionBuilder::createOutletConnection(*process_->store(), info, coreBroker_);
        } else if (className == "inletConnection") {
            return ConnectionBuilder::createInletConnection(*process_->store(), info, coreBroker_);
        }
        return Value::error(logger::error("CoreBroker::createObject({}, {}) failed. Class name is invalid.", className, info));
    }


  virtual Value deleteObject(const std::string& className, const std::string& fullName) override {
        logger::trace("CoreBroker::CoreFactoryBroker::deleteObject({})", fullName);
        if (className == "operation") {
            return ObjectFactory::deleteOperation(*process_->store(), fullName);
        } else if (className == "container") {
            return ObjectFactory::deleteContainer(*process_->store(), fullName);
        } else if (className == "containerOperation") {
            return ObjectFactory::deleteContainerOperation(*process_->store(), fullName);
        } else if (className == "ec") {
            return ObjectFactory::deleteExecutionContext(*process_->store(), fullName);
        //} else if (className == "fsm") {
        //    return ObjectFactory::deleteFSM(*process_->store(), fullName);
        }

        return Value::error(logger::error("CoreBroker::deleteObject({}, {}) failed. Class name is invalid.", className, fullName));
    }
};

class CoreStoreBroker : public StoreClientAPI {
private:
  ProcessAPI* process_;
public:
  CoreStoreBroker(ProcessAPI* proc) : process_(proc) {}
  virtual ~CoreStoreBroker() {}

  virtual Value getClassObjectInfos(const std::string& className) const override {
        logger::trace("CoreBroker::getClassObjectInfos({})", className);
        if (className == "operation") {
            return juiz::functional::map<Value, std::shared_ptr<OperationAPI>>(process_->store()->list<OperationAPI>(), [](auto op) { return op->info(); });
        } else if (className == "operationFactory") {
            return juiz::functional::map<Value, std::shared_ptr<OperationFactoryAPI>>(process_->store()->list<OperationFactoryAPI>(), [](auto op) { return op->info(); });
        } else if (className == "container") {
            return juiz::functional::map<Value, std::shared_ptr<ContainerAPI>>(process_->store()->list<ContainerAPI>(), [](auto o) { return o->info(); });
        } else if (className == "connection") {
            return juiz::functional::map<Value, std::shared_ptr<ConnectionAPI>>(process_->store()->connections(), [](auto o) { return o->info(); });
        } else if (className == "containerFactory") {
            return juiz::functional::map<Value, std::shared_ptr<ContainerFactoryAPI>>(process_->store()->list<ContainerFactoryAPI>(), [](auto o) { return o->info(); });
        //} else if (className == "ec") {
        //    return juiz::functional::map<Value, std::shared_ptr<ExecutionContextAPI>>(process_->store()->executionContexts(), [](auto o) { return o->info(); });
        } else if (className == "topic") {
            return juiz::functional::map<Value, std::shared_ptr<TopicAPI>>(process_->store()->list<TopicAPI>(), [](auto o) { return o->info(); });
        } else if (className == "ecFactory") {
            return juiz::functional::map<Value, std::shared_ptr<ExecutionContextFactoryAPI>>(process_->store()->executionContextFactories(), [](auto o) { return o->info(); });
        //} else if (className == "fsm") {
        //    return juiz::functional::map<Value, std::shared_ptr<FSMAPI>>(process_->store()->fsms(), [](auto o) { return o->info(); });
        //} else if (className == "fsmFactory") {
        //    return juiz::functional::map<Value, std::shared_ptr<FSMFactoryAPI>>(process_->store()->fsmFactories(), [](auto o) { return o->info(); });
        } else if (className == "broker") {
            return juiz::functional::map<Value, std::shared_ptr<BrokerAPI>>(process_->store()->brokers(), [](auto o) { return o->info(); });
        } else if (className == "brokerFactory") {
            return juiz::functional::map<Value, std::shared_ptr<BrokerFactoryAPI>>(process_->store()->brokerFactories(), [](auto o) { return o->info(); });
        }
        return Value::error(logger::error("CoreBroker::getClassObjectInfos({}) failed. Class name is invalid.", className));
    }


  virtual Value getChildrenClassObjectInfos(const std::string& parentName, const std::string& className) const override {
      return Value::error(logger::error("CoreBroker::getChildrenClassObjectInfos() failed. Not implemented"));
  }

  virtual Value getObjectInfo(const std::string& className, const std::string& fullName) const override {
    if (className == "operation") {
        return process_->store()->get<OperationAPI>(fullName)->info();
    } else if (className == "operationFactory") {
        return process_->store()->get<OperationFactoryAPI>(fullName)->info();
    } else if (className == "container") {
        return process_->store()->get<ContainerAPI>(fullName)->info();
    } else if (className == "containerFactory") {
        return process_->store()->get<ContainerFactoryAPI>(fullName)->info();
    } else if (className == "containerOperationFactory") {
        return process_->store()->get<ContainerOperationFactoryAPI>(fullName)->info();
    //} else if (className == "ec") {
    //    return process_->store()->executionContext(fullName)->info();
    } else if (className == "ecFactory") {
        return process_->store()->executionContextFactory(fullName)->info();
    } else if (className == "topic") {
        return process_->store()->get<TopicAPI>(fullName)->info();
    } else if (className == "topicFactory") {
        return process_->store()->get<TopicFactoryAPI>(fullName)->info();
    //} else if (className == "fsm") {
    //    return process_->store()->fsm(fullName)->info();
    //} else if (className == "fsmFactory") {
    //    return process_->store()->fsmFactory(fullName)->info();
    } else if (className == "broker") {
        return process_->store()->broker(fullName)->info();
    } else if (className == "brokerFactory") {
        return process_->store()->brokerFactory(fullName)->info();
    } else if (className == "connection") {
        return process_->store()->connection(fullName)->info();
    }
    return Value::error(logger::error("CoreStoreBroker::getObjectInfo({}, {}) failed. ClassName={} not found", className, fullName, className));
  }
};


class CoreOperationBroker : public OperationClientAPI {
private:
  ProcessAPI* process_;
public:
  CoreOperationBroker(ProcessAPI* proc) : process_(proc) {}
  virtual ~CoreOperationBroker() {}

  virtual Value fullInfo(const std::string& fullName) const override {
      return process_->store()->get<OperationAPI>(fullName)->fullInfo();
  }

  virtual Value call(const std::string& fullName, const Value& value) override {
      return process_->store()->get<OperationAPI>(fullName)->call(value);
  }

  virtual Value invoke(const std::string& fullName) override {
      return process_->store()->get<OperationAPI>(fullName)->invoke();
  }

  virtual Value execute(const std::string& fullName) override {
      return process_->store()->get<OperationAPI>(fullName)->execute();
  }

  virtual Value inlets(const std::string& fullName) const override {
      return juiz::functional::map<Value, std::shared_ptr<InletAPI>>(process_->store()->get<OperationAPI>(fullName)->inlets(), [](auto il) {
          return il->info();
      });
  }

};


class CoreOperationOutletBroker : public OutletClientAPI {
private:
  ProcessAPI* process_;
public:
  CoreOperationOutletBroker(ProcessAPI* proc) : process_(proc) {}
  virtual ~CoreOperationOutletBroker() {}

  virtual Value info(const std::string& fullName) const override {
      return process_->store()->get<OperationAPI>(fullName)->outlet()->info();
  }
  
  virtual Value get(const std::string& fullName) const override {
      return process_->store()->get<OperationAPI>(fullName)->outlet()->get();
  }

  virtual Value connections(const std::string& fullName) const override {
      return juiz::functional::map<Value, std::shared_ptr<ConnectionAPI>>(process_->store()->get<OperationAPI>(fullName)->outlet()->connections(), [](auto c) {
          return c->info();
      });
  }

  //virtual Value addConnection(const std::string& fullName, const Value& c) override {
  //    /// FSMからはこちらが呼ばれるのはなんとかしなければならない
  //    return process_->store()->get<OperationAPI>(fullName)->outlet()->addConnection(ProxyBuilder::outgoingOperationConnectionProxy(c, process_->store()));
  //}
  
  //virtual Value removeConnection(const std::string& fullName, const std::string& name) override {
  //    return process_->store()->get<OperationAPI>(fullName)->outlet()->removeConnection(name);
  //}

    virtual Value connectTo(const std::string& fullName, const Value& conInfo) override {
        logger::trace("CoreOperationOutletBroker({})::{}({}) called.", fullName, __func__, conInfo);
        // TODO:
        std::shared_ptr<juiz::InletAPI> inlet = nullptr;
        if (conInfo["inlet"].hasKey("fsm")) {
            inlet = process_->store()->operationProxy(conInfo["inlet"]["fsm"])->inlet(conInfo["inlet"]["name"].stringValue());
        } else if (conInfo["inlet"].hasKey("operation")) {
            inlet = process_->store()->operationProxy(conInfo["inlet"]["operation"])->inlet(conInfo["inlet"]["name"].stringValue());
        } else {
            return Value::error(logger::error("CoreOperationOutletBroker::{}({}) called. passed connection information is invalid ({})", __func__, fullName, conInfo));
        }
        
        return process_->store()->get<OperationAPI>(fullName)->outlet()->connectTo(inlet, conInfo);
        //return Value::error(logger::error("NullOperationOutletBroker::{}({}) called. Object is null.", __func__, fullName));
    }

    virtual Value disconnectFrom(const std::string& fullName, const Value& inletInfo) override {
        return Value::error(logger::error("NullOperationOutletBroker::{}({}) called. Object is null.", __func__, fullName));
    }
};

class CoreOperationInletBroker : public InletClientAPI {
private:
  ProcessAPI* process_;
public:
  CoreOperationInletBroker(ProcessAPI* proc) : process_(proc) {}
  virtual ~CoreOperationInletBroker() {}

  virtual Value name(const std::string& fullName, const std::string& targetName) const override {
      return process_->store()->get<OperationAPI>(fullName)->inlet(targetName)->name();
  }
  
  virtual Value info(const std::string& fullName, const std::string& targetName) const override {
      return process_->store()->get<OperationAPI>(fullName)->inlet(targetName)->info();
  }
  
  virtual Value defaultValue(const std::string& fullName, const std::string& targetName) const override {
      return process_->store()->get<OperationAPI>(fullName)->inlet(targetName)->defaultValue();
  }
  
  virtual Value put(const std::string& fullName, const std::string& targetName, const Value& value) const override {
      return process_->store()->get<OperationAPI>(fullName)->inlet(targetName)->put(value);
  }
  
  virtual Value get(const std::string& fullName, const std::string& targetName) const override {
      return process_->store()->get<OperationAPI>(fullName)->inlet(targetName)->get();
  }
  
  virtual Value isUpdated(const std::string& fullName, const std::string& targetName) const override {
      return process_->store()->get<OperationAPI>(fullName)->inlet(targetName)->isUpdated();
  }
  
  virtual Value connections(const std::string& fullName, const std::string& targetName) const override {
      return juiz::functional::map<Value, std::shared_ptr<ConnectionAPI>>(process_->store()->get<OperationAPI>(fullName)->inlet(targetName)->connections(),
         [](auto c) {
              return c->info();
          });
  }
  

  //virtual Value addConnection(const std::string& fullName, const std::string& targetName, const Value& c) override {
  //    logger::trace("CoreOperationInletBroker::{}({}, {}, {}) called", __func__, fullName, targetName, c);
  //    return process_->store()->get<OperationAPI>(fullName)->inlet(targetName)->addConnection(ProxyBuilder::incomingOperationConnectionProxy(c, process_->store()));
  //}
  
  
  //virtual Value removeConnection(const std::string& fullName, const std::string& targetName, const std::string& name) override {
  //    logger::trace("CoreOperationInletBroker::{}({}, {}, {}) called", __func__, fullName, targetName, name);
  //    auto inlet = juiz::functional::find<std::shared_ptr<OperationInletAPI>>(process_->store()->get<OperationAPI>(fullName)->inlets(), [&targetName](auto i) {
  //        return i->name() == targetName;
  //    });
  //    if (inlet) { return inlet.value()->removeConnection(name); }
  //    return Value::error(logger::error("CoreOperationInletBroker::removeConnection({}, {}) failed. Inlet can not be found.", fullName, targetName));
  //}
  
  // TODO: 未実装
     virtual Value connectTo(const std::string& fullName, const std::string& targetName, const Value& conInfo) override {
         auto outlet = process_->store()->operationProxy(conInfo["outlet"]["operation"])->outlet();
        return process_->store()->get<OperationAPI>(fullName)->inlet(targetName)->connectTo(outlet, conInfo);
    }


  // TODO: 未実装
    virtual Value disconnectFrom(const std::string& fullName, const std::string& targetName, const Value& outletInfo) override {
        return Value::error(logger::error("NullOperationInletBroker::{}({}) called. Object is null.", __func__, fullName));
    }
};


class CoreConnectionBroker : public ConnectionClientAPI {
private:
  ProcessAPI* process_;
public:
  CoreConnectionBroker(ProcessAPI* proc) : process_(proc) {}
  virtual ~CoreConnectionBroker() {}

    virtual Value createConnection(const Value& connectionInfo) override {
        logger::trace("CoreConnectionBroker::createConnection({}) called", connectionInfo);
        //if (connectionInfo.at("inlet").hasKey("fsm")) {
        //    return ConnectionBuilder::createStateBind(*process_->store(), connectionInfo);
        //} else {
            return ConnectionBuilder::createOperationConnection(*process_->store(), connectionInfo);
        //}
    }

    virtual Value deleteConnection(const std::string& fullName) override {
        logger::trace("CoreConnectionBroker::deleteConnection({}) called", fullName);
        return ConnectionBuilder::deleteConnection(process_->store(), fullName);
    }
        
};


class CoreContainerBroker : public ContainerClientAPI {
private:
  ProcessAPI* process_;
public:
  CoreContainerBroker(ProcessAPI* proc) : process_(proc) {}
  virtual ~CoreContainerBroker() {}

  virtual Value operations(const std::string& containerFullName) const override {
    return juiz::functional::map<Value, std::shared_ptr<OperationAPI>>(process_->store()->get<ContainerAPI>(containerFullName)->operations(), [](auto op) {
        return op->info();
    });
  }

  virtual Value fullInfo(const std::string& containerFullName) const override {
    return process_->store()->get<ContainerAPI>(containerFullName)->fullInfo();
  }
};


/**
 * 
 */
CoreBroker::CoreBroker(ProcessAPI* process, const std::string& fullName): 
ClientProxyAPI("CoreBroker", "CoreBroker", fullName,
    std::make_shared<CoreStoreBroker>(process),
    std::make_shared<CoreFactoryBroker>(process, this), 
    std::make_shared<CoreOperationBroker>(process),
    std::make_shared<CoreOperationOutletBroker>(process),
    std::make_shared<CoreOperationInletBroker>(process),
    std::make_shared<CoreConnectionBroker>(process),
    std::make_shared<CoreContainerBroker>(process)
), process_(process)
{}

CoreBroker::~CoreBroker() {}


Value CoreBroker::getProcessInfo() const{ return process_->info(); }

Value CoreBroker::getProcessFullInfo() const { return process_->fullInfo(); }

