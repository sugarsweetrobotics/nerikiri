#include <utility>

#include "nerikiri/process.h"
#include "nerikiri/corebroker.h"
#include "nerikiri/operation.h"
#include "nerikiri/connection.h"
#include "nerikiri/object_mapper.h"
#include "nerikiri/objectfactory.h"
#include "nerikiri/connection/connection_builder.h"

#include <nerikiri/proxy_builder.h>

using namespace nerikiri;




class CoreBroker : public BrokerProxyAPI {
protected:
  Process* process_;
public:
  std::shared_ptr<FactoryBrokerAPI> factory_;
  std::shared_ptr<StoreBrokerAPI> store_;
  std::shared_ptr<OperationBrokerAPI> operation_;
  std::shared_ptr<OperationOutletBrokerAPI> operationOutlet_;
  std::shared_ptr<OperationInletBrokerAPI> operationInlet_;
  std::shared_ptr<ConnectionBrokerAPI> connection_;
public:

  virtual std::shared_ptr<FactoryBrokerAPI> factory() override { return factory_; }
  virtual std::shared_ptr<StoreBrokerAPI>  store() override { return store_; }
  virtual std::shared_ptr<const FactoryBrokerAPI> factory() const override{ return factory_; }
  virtual std::shared_ptr<const StoreBrokerAPI> store() const override{ return store_; }
  virtual std::shared_ptr<OperationBrokerAPI>   operation() override { return operation_; }
  virtual std::shared_ptr<const OperationBrokerAPI>   operation() const override { return operation_; }
  virtual std::shared_ptr<OperationOutletBrokerAPI>   operationOutlet() override { return operationOutlet_; }
  virtual std::shared_ptr<const OperationOutletBrokerAPI>   operationOutlet() const override { return operationOutlet_; }
  virtual std::shared_ptr<OperationInletBrokerAPI>   operationInlet() override { return operationInlet_; }
  virtual std::shared_ptr<const OperationInletBrokerAPI>   operationInlet() const override { return operationInlet_; }
  virtual std::shared_ptr<ConnectionBrokerAPI>   connection() override { return connection_; }
  virtual std::shared_ptr<const ConnectionBrokerAPI>   connection() const override { return connection_; }

public:

  /**
   * 
   */
  CoreBroker(Process* process, const std::string& fullName);

  virtual ~CoreBroker();

    virtual Value getProcessInfo() const override;
    virtual Value getProcessFullInfo() const override;
  /*
    virtual Value getBrokerInfo() const override { return info_; }



    virtual Value getOperationInfos() const override;

    virtual Value getOperationInfo(const std::string& fullName) const override;

    virtual Value callOperation(const std::string& fullName, const Value& value) override;

    virtual Value invokeOperation(const std::string& fullName) const override;

    virtual Value executeOperation(const std::string& fullName) override;

    virtual Value getOperationConnectionInfos(const std::string& fullName) const override;

    virtual Value getOperationFactoryInfos() const override;



    virtual Value getContainerInfos() const override;

    virtual Value getContainerInfo(const std::string& fullName) const override;

    virtual Value getContainerOperationInfos(const std::string& fullName) const override;


    virtual Value getContainerFactoryInfos() const override;



    virtual Value getContainerOperationInfo(const std::string& fullName) const override;

    virtual Value getContainerOperationConnectionInfos(const std::string& fullName) const override;

    virtual Value callContainerOperation(const std::string& fullName, const Value& arg) override;

    virtual Value invokeContainerOperation(const std::string& fullName) const override;

    virtual Value executeContainerOperation(const std::string& fullName) override;

    virtual Value createContainerOperation(const std::string& fullName, const Value& value) override;

    virtual Value deleteContainerOperation(const std::string& fullName) override;



    virtual Value getAllOperationInfos() const override;

    virtual Value getAllOperationInfo(const std::string& fullName) const override;

    virtual Value invokeAllOperation(const std::string& fullName) const override;

    virtual Value executeAllOperation(const std::string& fullName) override;

    virtual Value callAllOperation(const std::string& fullName, const Value& value) override;

    virtual Value getAllOperationConnectionInfos(const std::string& fullName) const override;


    virtual Value getConnectionInfos() const override;

    virtual Value createConnection(const Value& connectionInfo, const Value& brokerInfo) override;

    virtual Value registerConsumerConnection(const Value& ci) override;

    virtual Value registerProviderConnection(const Value& ci) override;

    virtual Value removeConsumerConnection(const std::string& fullName, const std::string& targetArgName, const std::string& conName) override;

    virtual Value removeProviderConnection(const std::string& fullName, const std::string& conName) override;

    virtual Value putToArgument(const std::string& fullName, const std::string& argName, const Value& value) override;

    virtual Value putToArgumentViaConnection(const std::string& fullName, const std::string& targetArgName, const std::string& conName, const Value& value) override;



    virtual Value createResource(const std::string& path, const Value& value) override;

    virtual Value readResource(const std::string& path) const override;

    virtual Value updateResource(const std::string& path, const Value& value) override;

    virtual Value deleteResource(const std::string& path) override;


    virtual Value getExecutionContextInfos() const override;

    virtual Value getExecutionContextInfo(const std::string& fullName) const override;

    virtual Value getExecutionContextState(const std::string& fullName) const override;

    virtual Value setExecutionContextState(const std::string& fullName, const std::string& state) override;

    virtual Value getExecutionContextBoundOperationInfos(const std::string& fullName) const override;

    virtual Value getExecutionContextBoundAllOperationInfos(const std::string& fullName) const override;

    virtual Value bindOperationToExecutionContext(const std::string& ecFullName, const std::string& opFullName, const Value& opInfo) override;

    virtual Value unbindOperationFromExecutionContext(const std::string& ecFullName, const std::string& opFullName) override;

    virtual Value getBrokerInfos() const override;

    virtual Value getCallbacks() const override;

    virtual Value getTopicInfos() const override;

    virtual Value invokeTopic(const std::string& fullName) const override;
    
    virtual Value getTopicConnectionInfos(const std::string& fullName) const override;

    virtual Value getExecutionContextFactoryInfos() const override;

    virtual Value createExecutionContext(const Value& value) override;

    virtual Value deleteExecutionContext(const std::string& fullName) override;


    virtual Value getFSMInfos() const override;
    
    virtual Value getFSMInfo(const std::string& fullName) const override;

    virtual Value setFSMState(const std::string& fullName, const std::string& state) override;
    
    virtual Value getFSMState(const std::string& fullName) const override;

    virtual Value getOperationsBoundToFSMState(const std::string& fsmFullName, const std::string& state) override;

    virtual Value getECsBoundToFSMState(const std::string& fsmFullName, const std::string& state) override;

    virtual Value getFSMsBoundToFSMState(const std::string& fsmFullName, const std::string& state) override;

    virtual Value bindOperationToFSMState(const std::string& fsmFullName, const std::string& state, const Value& operation) override;

    virtual Value bindECStateToFSMState(const std::string& fsmFullName, const std::string& state, const Value& ecState) override;

    virtual Value bindFSMStateToFSMState(const std::string& fsmFullName, const std::string& state, const Value& fsmState) override;
 */
};


class CoreBrokerFactory : public nerikiri::BrokerFactoryAPI {
private:
  std::shared_ptr<CoreBroker> coreBroker_;

public:
  //virtual std::string brokerTypeFullName() const override {
  //    return "CoreBroker";
  //}


  CoreBrokerFactory(const std::shared_ptr<CoreBroker>& coreBroker):
   BrokerFactoryAPI("CoreBrokerFactory", "CoreBroker", "coreBrokerFactory"), coreBroker_(coreBroker) {}
  virtual ~CoreBrokerFactory() {}

public:
  virtual std::shared_ptr<BrokerAPI> create(const Value& param) override { 
    return std::make_shared<NullBroker>(); 
  }

  virtual std::shared_ptr<BrokerProxyAPI> createProxy(const Value& param) override {
      return coreBroker_;
  }
};



std::shared_ptr<BrokerFactoryAPI> nerikiri::coreBrokerFactory(Process* process, const std::string& fullName) {
    return std::make_shared<CoreBrokerFactory>(std::make_shared<CoreBroker>(process, fullName));
}

class CoreFactoryBroker : public FactoryBrokerAPI {
private:
  Process* process_;
public:
  CoreFactoryBroker(Process* proc) : process_(proc) {}
  virtual ~CoreFactoryBroker() {}

  virtual Value createObject(const std::string& className, const Value& info={}) override {
        logger::trace("CoreBroker::CoreFactoryBroker::createObject({}, {})", className, info);
        if (className == "operation") {
            return ObjectFactory::createOperation(*process_->store(), info);
        } else if (className == "container") {
            return ObjectFactory::createContainer(*process_->store(), info);
        } else if (className == "containerOperation") {
            return ObjectFactory::createContainerOperation(*process_->store(), info);
        } else if (className == "ec") {
            return ObjectFactory::createExecutionContext(*process_->store(), info);
        } else if (className == "fsm") {
            return ObjectFactory::createFSM(*process_->store(), info);
        } else if (className == "connection") {
            return ConnectionBuilder::createConnection(process_->store(), info);
        } else if (className == "outletConnection") {
            return ConnectionBuilder::createOutletConnection(process_->store(), info, process_->coreBroker());
        } else if (className == "inletConnection") {
            return ConnectionBuilder::createInletConnection(process_->store(), info, process_->coreBroker());
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
        } else if (className == "fsm") {
            return ObjectFactory::deleteFSM(*process_->store(), fullName);
        }

        return Value::error(logger::error("CoreBroker::deleteObject({}, {}) failed. Class name is invalid.", className, fullName));
    }
};

class CoreStoreBroker : public StoreBrokerAPI {
private:
  Process* process_;
public:
  CoreStoreBroker(Process* proc) : process_(proc) {}
  virtual ~CoreStoreBroker() {}

  virtual Value getClassObjectInfos(const std::string& className) const override {
        logger::trace("CoreBroker::getClassObjectInfos({})", className);
        if (className == "operation") {
            return nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(process_->store()->operations(), [](auto op) { return op->info(); });
        } else if (className == "operationFactory") {
            return nerikiri::functional::map<Value, std::shared_ptr<OperationFactoryAPI>>(process_->store()->operationFactories(), [](auto op) { return op->info(); });
        } else if (className == "container") {
            return nerikiri::functional::map<Value, std::shared_ptr<ContainerAPI>>(process_->store()->containers(), [](auto o) { return o->info(); });
        } else if (className == "containerFactory") {
            return nerikiri::functional::map<Value, std::shared_ptr<ContainerFactoryAPI>>(process_->store()->containerFactories(), [](auto o) { return o->info(); });
        } else if (className == "ec") {
            return nerikiri::functional::map<Value, std::shared_ptr<ExecutionContextAPI>>(process_->store()->executionContexts(), [](auto o) { return o->info(); });
        } else if (className == "topic") {
            return nerikiri::functional::map<Value, std::shared_ptr<TopicAPI>>(process_->store()->topics(), [](auto o) { return o->info(); });
        } else if (className == "ecFactory") {
            return nerikiri::functional::map<Value, std::shared_ptr<ExecutionContextFactoryAPI>>(process_->store()->executionContextFactories(), [](auto o) { return o->info(); });
        } else if (className == "fsm") {
            return nerikiri::functional::map<Value, std::shared_ptr<FSMAPI>>(process_->store()->fsms(), [](auto o) { return o->info(); });
        } else if (className == "fsmFactory") {
            return nerikiri::functional::map<Value, std::shared_ptr<FSMFactoryAPI>>(process_->store()->fsmFactories(), [](auto o) { return o->info(); });
        } else if (className == "broker") {
            return nerikiri::functional::map<Value, std::shared_ptr<BrokerAPI>>(process_->store()->brokers(), [](auto o) { return o->info(); });
        } else if (className == "brokerFactory") {
            return nerikiri::functional::map<Value, std::shared_ptr<BrokerFactoryAPI>>(process_->store()->brokerFactories(), [](auto o) { return o->info(); });
        }
        return Value::error(logger::error("CoreBroker::getClassObjectInfos({}) failed. Class name is invalid.", className));
    }


  virtual Value getChildrenClassObjectInfos(const std::string& parentName, const std::string& className) const override {}

  virtual Value getObjectInfo(const std::string& className, const std::string& fullName) const override {}
};


class CoreOperationBroker : public OperationBrokerAPI {
private:
  Process* process_;
public:
  CoreOperationBroker(Process* proc) : process_(proc) {}
  virtual ~CoreOperationBroker() {}

  virtual Value fullInfo(const std::string& fullName) const override {
      return process_->store()->operation(fullName)->fullInfo();
  }

  virtual Value call(const std::string& fullName, const Value& value) override {
      return process_->store()->operation(fullName)->call(value);
  }

  virtual Value invoke(const std::string& fullName) override {
      return process_->store()->operation(fullName)->invoke();
  }

  virtual Value execute(const std::string& fullName) override {
      return process_->store()->operation(fullName)->execute();
  }

  virtual Value inlets(const std::string& fullName) const override {
      return nerikiri::functional::map<Value, std::shared_ptr<OperationInletAPI>>(process_->store()->operation(fullName)->inlets(), [](auto il) {
          return il->info();
      });
  }

};


class CoreOperationOutletBroker : public OperationOutletBrokerAPI {
private:
  Process* process_;
public:
  CoreOperationOutletBroker(Process* proc) : process_(proc) {}
  virtual ~CoreOperationOutletBroker() {}

  virtual Value info(const std::string& fullName) const override {
      return process_->store()->operation(fullName)->outlet()->info();
  }

  virtual Value get(const std::string& fullName) const override {
      return process_->store()->operation(fullName)->outlet()->get();
  }

  virtual Value connections(const std::string& fullName) const override {
      return nerikiri::functional::map<Value, std::shared_ptr<ConnectionAPI>>(process_->store()->operation(fullName)->outlet()->connections(), [](auto c) {
          return c->info();
      });
  }

  virtual Value addConnection(const std::string& fullName, const Value& c) override {
      return process_->store()->operation(fullName)->outlet()->addConnection(ProxyBuilder::outgoingConnectionProxy(c, process_->store()));
  }
  
  virtual Value removeConnection(const std::string& fullName, const std::string& name) override {
      return process_->store()->operation(fullName)->outlet()->removeConnection(name);
  }
};

class CoreOperationInletBroker : public OperationInletBrokerAPI {
private:
  Process* process_;
public:
  CoreOperationInletBroker(Process* proc) : process_(proc) {}
  virtual ~CoreOperationInletBroker() {}

  virtual Value name(const std::string& fullName, const std::string& targetName) const override {
      auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(process_->store()->operation(fullName)->inlets(), [&targetName](auto i) {
          return i->name() == targetName;
      });
      if (inlet) { return inlet.value()->name(); }
      return Value::error(logger::error("CoreOperationInletBroker::name({}, {}) failed. Inlet can not be found.", fullName, targetName));
  }
  
  virtual Value info(const std::string& fullName, const std::string& targetName) const override {
      auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(process_->store()->operation(fullName)->inlets(), [&targetName](auto i) {
          return i->name() == targetName;
      });
      if (inlet) { return inlet.value()->info(); }
      return Value::error(logger::error("CoreOperationInletBroker::name({}, {}) failed. Inlet can not be found.", fullName, targetName));
  }
  

  virtual Value defaultValue(const std::string& fullName, const std::string& targetName) const override {
      auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(process_->store()->operation(fullName)->inlets(), [&targetName](auto i) {
          return i->name() == targetName;
      });
      if (inlet) { return inlet.value()->defaultValue(); }
      return Value::error(logger::error("CoreOperationInletBroker::name({}, {}) failed. Inlet can not be found.", fullName, targetName));
  }
  

  virtual Value put(const std::string& fullName, const std::string& targetName, const Value& value) const override {
      auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(process_->store()->operation(fullName)->inlets(), [&targetName](auto i) {
          return i->name() == targetName;
      });
      if (inlet) { return inlet.value()->put(value); }
      return Value::error(logger::error("CoreOperationInletBroker::name({}, {}) failed. Inlet can not be found.", fullName, targetName));
  }
  

  virtual Value get(const std::string& fullName, const std::string& targetName) const override {
      auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(process_->store()->operation(fullName)->inlets(), [&targetName](auto i) {
          return i->name() == targetName;
      });
      if (inlet) { return inlet.value()->get(); }
      return Value::error(logger::error("CoreOperationInletBroker::name({}, {}) failed. Inlet can not be found.", fullName, targetName));
  }
  

  virtual Value isUpdated(const std::string& fullName, const std::string& targetName) const override {
      auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(process_->store()->operation(fullName)->inlets(), [&targetName](auto i) {
          return i->name() == targetName;
      });
      if (inlet) { return inlet.value()->isUpdated(); }
      return Value::error(logger::error("CoreOperationInletBroker::name({}, {}) failed. Inlet can not be found.", fullName, targetName));
  }
  

  virtual Value connections(const std::string& fullName, const std::string& targetName) const override {
      auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(process_->store()->operation(fullName)->inlets(), [&targetName](auto i) {
          return i->name() == targetName;
      });
      if (inlet) { 
          return nerikiri::functional::map<Value, std::shared_ptr<ConnectionAPI>>(inlet.value()->connections(), [](auto c) {
              return c->info();
          });
      }
      return Value::error(logger::error("CoreOperationInletBroker::name({}, {}) failed. Inlet can not be found.", fullName, targetName));
  }
  

  virtual Value addConnection(const std::string& fullName, const std::string& targetName, const Value& c) override {
      auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(process_->store()->operation(fullName)->inlets(), [&targetName](auto i) {
          return i->name() == targetName;
      });
      if (inlet) { 
          return inlet.value()->addConnection(ProxyBuilder::incomingConnectionProxy(c, process_->store()));
      }
      return Value::error(logger::error("CoreOperationInletBroker::name({}, {}) failed. Inlet can not be found.", fullName, targetName));
  }
  
  
  virtual Value removeConnection(const std::string& fullName, const std::string& targetName, const std::string& name) override {
      auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(process_->store()->operation(fullName)->inlets(), [&targetName](auto i) {
          return i->name() == targetName;
      });
      if (inlet) { return inlet.value()->removeConnection(name); }
      return Value::error(logger::error("CoreOperationInletBroker::name({}, {}) failed. Inlet can not be found.", fullName, targetName));
  }
  
};


class CoreConnectionBroker : public ConnectionBrokerAPI {
private:
  Process* process_;
public:
  CoreConnectionBroker(Process* proc) : process_(proc) {}
  virtual ~CoreConnectionBroker() {}

    virtual Value createConnection(const Value& connectionInfo) override {
        return ConnectionBuilder::createConnection(process_->store(), connectionInfo);
    }

    virtual Value deleteConnection(const std::string& fullName) override {
    }
        
};

/**
 * 
 */
CoreBroker::CoreBroker(Process* process, const std::string& fullName): BrokerProxyAPI("CoreBroker", fullName), process_(process),
factory_(std::make_shared<CoreFactoryBroker>(process)), 
store_(std::make_shared<CoreStoreBroker>(process)),
operation_(std::make_shared<CoreOperationBroker>(process)),
operationInlet_(std::make_shared<CoreOperationInletBroker>(process)),
operationOutlet_(std::make_shared<CoreOperationOutletBroker>(process)),
connection_(std::make_shared<CoreConnectionBroker>(process))
{}

CoreBroker::~CoreBroker() {}


Value CoreBroker::getProcessInfo() const{ return process_->info(); }

Value CoreBroker::getProcessFullInfo() const { return process_->fullInfo(); }


//std::shared_ptr<BrokerAPI>  Broker::null = std::shared_ptr<BrokerAPI>(nullptr);

/*

Value CoreBroker::getOperationInfos() const {
    return nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(process_->store()->operations(), [](const auto& op) -> Value { return op->info(); });
}

Value CoreBroker::getAllOperationInfos() const {
    //return process_->store()->getAllOperationInfos();
    return getOperationInfos();
}

Value CoreBroker::getContainerInfos() const {
    //return process_->store()->getContainerInfos();
    return nerikiri::functional::map<Value, std::shared_ptr<ContainerAPI>>(process_->store()->containers(), [](const auto& c) { return c->info(); });
}

Value CoreBroker::getContainerInfo(const std::string& fullName) const {
    return process_->store()->container(fullName)->info();
    //return process_->store()->getContainer(fullName)->info();
}

Value CoreBroker::getContainerOperationInfos(const std::string& fullName) const {
    return nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(process_->store()->container(fullName)->operations(), [](auto op) { return op->info(); });
    //return process_->store()->getContainer(fullName)->getOperationInfos();
}

Value CoreBroker::getContainerOperationInfo(const std::string& fullName) const {
    return process_->store()->operation(fullName)->info();
    //auto [containerName, operationName] = splitContainerAndOperationName(fullName);
    //return process_->store()->getContainer(containerName)->getOperation(operationName)->info();
}

Value CoreBroker::getOperationInfo(const std::string& fullName) const {
    //return process_->store()->getAllOperation(fullName)->info();
    return process_->store()->container(fullName)->info();
}

Value CoreBroker::callContainerOperation(const std::string& fullName, const Value& arg) {
    return process_->store()->operation(fullName)->call(arg);
    //auto [containerName, operationName] = splitContainerAndOperationName(fullName);
    //return process_->store()->getContainer(containerName)->getOperation(operationName)->call(std::move(arg));
    //return process_->store()->container(containerName)->operation(operationName)->call(std::move(arg));
}

Value CoreBroker::invokeContainerOperation(const std::string& fullName) const {
    return process_->store()->operation(fullName)->invoke();
    //auto [containerName, operationName] = splitContainerAndOperationName(fullName);
    //return process_->store()->getContainer(containerName)->getOperation(operationName)->invoke();
}

Value CoreBroker::callOperation(const std::string& fullName, const Value& value) {
    return process_->store()->operation(fullName)->call(value);
}

Value CoreBroker::invokeOperation(const std::string& fullName) const {
    //return process_->store()->getAllOperation(fullName)->invoke();
    return process_->store()->operation(fullName)->invoke();
}

Value CoreBroker::executeOperation(const std::string& fullName) {
    //logger::trace("CoreBroker::executeOperation({})", fullName);
    //return process_->store()->getAllOperation(fullName)->execute();
    return process_->store()->operation(fullName)->execute();
}


Value CoreBroker::callAllOperation(const std::string& fullName, const Value& value) {
    //return process_->store()->getAllOperation(fullName)->call(std::move(value));
    return process_->store()->operation(fullName)->call(value);
}

Value CoreBroker::invokeAllOperation(const std::string& fullName) const {
    logger::trace("CoreBroker::invokeAllOperation({})", fullName);
    //return process_->store()->getAllOperation(fullName)->invoke();
    return process_->store()->operation(fullName)->invoke();
}

Value CoreBroker::executeAllOperation(const std::string& fullName) {
    logger::trace("CoreBroker::executeAllOperation({})", fullName);
    //return process_->store()->getAllOperation(fullName)->execute();
    return process_->store()->operation(fullName)->execute();
}


Value CoreBroker::getConnectionInfos() const {
    std::vector<std::shared_ptr<ConnectionAPI>> connections;
    for(auto op : process_->store()->operations()) {
        auto cons = op->outlet()->connections();
        connections.insert(connections.end(), cons.begin(), cons.end());
    }
    return nerikiri::functional::map<Value, std::shared_ptr<ConnectionAPI>>(connections, [](auto c) { return c->info(); });
}

Value CoreBroker::createConnection(const Value& connectionInfo, const Value& brokerInfo) {
    logger::trace("CoreBroker::registerConsumerConnection({}");
    return ConnectionBuilder::createConnection(process_->store(), connectionInfo);
}


Value CoreBroker::registerConsumerConnection(const Value& ci) {
    logger::trace("CoreBroker::registerConsumerConnection({}", str(ci));
    return ConnectionBuilder::registerConsumerConnection(process_->store(), ci);
}

Value CoreBroker::registerProviderConnection(const Value& ci) {
    logger::trace("CoreBroker::registerProviderConnection({}", str(ci));
    return ConnectionBuilder::registerProviderConnection(process_->store(), ci);
}

Value CoreBroker::removeConsumerConnection(const std::string& operationFullName, const std::string& targetArgName, const std::string& conName) {
    logger::trace("CoreBroker::removeConsumerConnection({}", conName);
    return ConnectionBuilder::deleteConsumerConnection(process_->store(), operationFullName, targetArgName, conName);
}

Value CoreBroker::removeProviderConnection(const std::string& operationFullName, const std::string& conName) {
    logger::trace("CoreBroker::removeProviderConnection({}", conName);
    return ConnectionBuilder::deleteProviderConnection(process_->store(), operationFullName, conName);
}

Value CoreBroker::putToArgument(const std::string& fullName, const std::string& argName, const Value& value) {
    logger::trace("CoreBroker::putToArgument()");
    return process_->store()->getOperationOrTopic(fullName)->putToArgument(argName, value);
}

Value CoreBroker::putToArgumentViaConnection(const std::string& fullName, const std::string& argName, const std::string& conName, const Value& value) {
    logger::trace("CoreBroker::putToArgumentViaConnection({})", conName);
    return process_->store()->getOperationOrTopic(fullName)->putToArgumentViaConnection(
        argName, conName, value);
}

Value CoreBroker::getOperationFactoryInfos() const {
    logger::trace("CoreBroker::getOperationFactoryInfos()");
    return process_->store()->getOperationFactoryInfos();
}

Value CoreBroker::getContainerFactoryInfos() const {
    logger::trace("CoreBroker::getContainerFactoryInfos()");
    return process_->store()->getContainerFactoryInfos();
}


Value CoreBroker::createOperation(const Value& value) {
}

Value CoreBroker::createContainer(const Value& value) {
    logger::trace("CoreBroker::createContainer({})", value);
}


Value CoreBroker::createContainerOperation(const std::string& fullName, const Value& value) {
    logger::trace("Broker::createContainerOperation({})", value);
    return process_->store()->getContainer(fullName)->createContainerOperation(value);
}

Value CoreBroker::deleteOperation(const std::string& fullName) {
    logger::trace("Broker::deleteOperation({})", fullName);
    return ObjectFactory::deleteOperation(*process_->store(), fullName);
}

Value CoreBroker::deleteContainer(const std::string& fullName) {
    logger::trace("CoreBroker::deleteContainer({})", fullName);
    return ObjectFactory::deleteContainer(*process_->store(), fullName);
    
}

Value CoreBroker::deleteContainerOperation(const std::string& fullName) { 
    logger::trace("CoreBroker::deleteContainerOperation({})", fullName);
    auto [containerName, operationName] = splitContainerAndOperationName(fullName);
    return process_->store()->getContainer(containerName)->deleteContainerOperation(operationName);
}

Value CoreBroker::createExecutionContext(const Value& value) {
    logger::trace("CoreBroker::createExecutionContext({})", value);
    return ObjectFactory::createExecutionContext(*process_->store(), value);
    
}
Value CoreBroker::deleteExecutionContext(const std::string& fullName) {
    logger::trace("CoreBroker::deleteExecutionContext({})", fullName);
    return ObjectFactory::deleteExecutionContext(*process_->store(), fullName);
}

Value CoreBroker::createResource(const std::string& path, const Value& value) {
    //return process_->createResource(path, value);
    return ObjectMapper::createResource(this, path, std::move(value));
}

Value CoreBroker::readResource(const std::string& path) const {
    //return process_->readResource(path);//
    return ObjectMapper::readResource(this, path);
}

Value CoreBroker::updateResource(const std::string& path, const Value& value) {
    //return process_->updateResource(path, value);
    return ObjectMapper::updateResource(this, path, std::move(value));
}

Value CoreBroker::deleteResource(const std::string& path) {
    //return process_->deleteResource(path);
    return ObjectMapper::deleteResource(this, path);
}


Value CoreBroker::setExecutionContextState(const std::string& fullName, const std::string& state) {
    logger::trace("CoreBroker::setExecutionContextState({}, {})", fullName, state);
    return process_->store()->getExecutionContext(fullName)->setState(state);
}

Value CoreBroker::executeContainerOperation(const std::string& fullName) {
    //return process_->store()->getContainerOperation(fullName)->execute();
    return process_->store()->operation(fullName)->execute();
}

Value CoreBroker::bindOperationToExecutionContext(const std::string& ecFullName, const std::string& opFullName, const Value& brokerInfo) {
    //auto broker = process_->store()->getBrokerFactory(brokerInfo)->createProxy(brokerInfo);
    //return process_->store()->getExecutionContext(ecFullName)->bind(opFullName, broker);
    return process_->store()->getExecutionContext(ecFullName)->bind(process_->store()->getAllOperation(opFullName));
}

Value CoreBroker::unbindOperationFromExecutionContext(const std::string& ecFullName, const std::string& opFullName) {
    return process_->store()->getExecutionContext(ecFullName)->unbind(opFullName);
}

Value CoreBroker::getBrokerInfos() const {
    return process_->store()->getBrokerInfos();
}

Value CoreBroker::getCallbacks() const {
    return process_->store()->getCallbacks();
}

Value CoreBroker::getTopicInfos() const {
    return process_->store()->getTopicInfos();
}

Value CoreBroker::invokeTopic(const std::string& fullName) const {
    return process_->store()->getTopic(fullName)->invoke();
}

Value CoreBroker::getExecutionContextInfos() const {
    return process_->store()->getExecutionContextInfos();
}

Value CoreBroker::getAllOperationInfo(const std::string& fullName) const {
    //return process_->store()->getAllOperation(fullName)->info();
    return nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(
        process_->store()->operations(), [](auto op) { return op->info(); });
    
}

Value CoreBroker::getExecutionContextInfo(const std::string& fullName) const {
    return process_->store()->getExecutionContext(fullName)->info();
}

Value CoreBroker::getTopicConnectionInfos(const std::string& fullName) const {
    return process_->store()->getTopic(fullName)->getConnectionInfos();
}

Value CoreBroker::getExecutionContextState(const std::string& fullName) const {
    return process_->store()->getExecutionContext(fullName)->getObjectState();
}

Value CoreBroker::getOperationConnectionInfos(const std::string& fullName) const {
    return process_->store()->getOperation(fullName)->getConnectionInfos();
}

Value CoreBroker::getAllOperationConnectionInfos(const std::string& fullName) const {
    return process_->store()->getAllOperation(fullName)->getConnectionInfos();
}

Value CoreBroker::getExecutionContextFactoryInfos() const {
    return process_->store()->getExecutionContextFactoryInfos();
}

Value CoreBroker::getContainerOperationConnectionInfos(const std::string& fullName) const {
    return process_->store()->getContainerOperation(fullName)->getConnectionInfos();
}

Value CoreBroker::getExecutionContextBoundOperationInfos(const std::string& fullName) const {
    return process_->store()->getExecutionContext(fullName)->getBoundOperationInfos();
}

Value CoreBroker::getExecutionContextBoundAllOperationInfos(const std::string& fullName) const {
    return process_->store()->getExecutionContext(fullName)->getBoundOperationInfos();
}


Value CoreBroker::getFSMInfos() const {
    logger::trace("CoreBroker::getFSMInfos() called");
    return nerikiri::functional::map<Value, std::shared_ptr<FSMAPI>>(process_->store()->fsms(), [](auto f) {
        return f->info();
    });
}

Value CoreBroker::getFSMInfo(const std::string& fullName) const {
    //return process_->store()->getFSM(fullName)->info();
    logger::trace("CoreBroker::getFSMInfo({}) called", fullName);
    return process_->store()->fsm(fullName)->info();
}

Value CoreBroker::setFSMState(const std::string& fullName, const std::string& state) {
    logger::trace("CoreBroker::setFSMState({}, {})", fullName, state);
    ///return process_->store()->getFSM(fullName)->setFSMState(state);
    return process_->store()->fsm(fullName)->setFSMState(state);
}

Value CoreBroker::getFSMState(const std::string& fullName) const {
    //return process_->store()->getFSM(fullName)->getFSMState();
    logger::trace("CoreBroker::getFSMState({})", fullName);
    return process_->store()->fsm(fullName)->currentFsmState()->info();
}

Value CoreBroker::getOperationsBoundToFSMState(const std::string& fsmFullName, const std::string& stateName) {
    return nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(process_->store()->fsm(fsmFullName)->fsmState(stateName)->boundOperations(), [](auto op) {
        return op->info();
    });
    //auto ops = process_->store()->getFSM(fsmFullName)->getBoundOperations(state);
    //Value v = Value::list();
    //for(auto op : ops) {
    //    v.push_back(op->info());
    //}
    //return v;
}

Value CoreBroker::getECsBoundToFSMState(const std::string& fsmFullName, const std::string& state) {
    auto ecAndStates = process_->store()->getFSM(fsmFullName)->getBoundECs(state);
    Value v = Value::list();
    for(auto ecAndState : ecAndStates) {
        if (ecAndState.first == state) {
            v.push_back(ecAndState.second->info());
        }
    }
    return v;
}


Value CoreBroker::getFSMsBoundToFSMState(const std::string& fsmFullName, const std::string& state) {
    
}

Value CoreBroker::bindOperationToFSMState(const std::string& fsmFullName, const std::string& state, const Value& operation) {
    
}

Value CoreBroker::bindECStateToFSMState(const std::string& fsmFullName, const std::string& state, const Value& ecState) {
    
}

Value CoreBroker::bindFSMStateToFSMState(const std::string& fsmFullName, const std::string& state, const Value& fsmState) {
    
}
*/
