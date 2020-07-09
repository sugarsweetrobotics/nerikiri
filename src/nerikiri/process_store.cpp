
#include <sstream>
#include "nerikiri/nerikiri.h"
#include "nerikiri/functional.h"
#include "nerikiri/logger.h"
#include "nerikiri/signal.h"
#include "nerikiri/naming.h"

#include "nerikiri/process.h"

#include "nerikiri/broker.h"
#include <iostream>

using namespace nerikiri;
using namespace nerikiri::logger;


Value ProcessStore::info() const { return process_->info(); }

Value ProcessStore::getContainerInfos() {
  return {nerikiri::map<Value, std::shared_ptr<ContainerBase>>(containers_, [](auto& ctn) { return ctn->info(); })};
}

/**
 * Containerの追加．fullNameやinstanceNameの自動割り当ても行う
 */
Value ProcessStore::addContainer(const std::shared_ptr<ContainerBase>& container) {
  return add<ContainerBase>(containers_, container, ".ctn");
}

/**
 * Containerの取得
 */
std::shared_ptr<ContainerBase> ProcessStore::getContainer(const std::string& fullName) {
  return get<ContainerBase>(containers_, fullName, nullContainer);
}

/**
 * Operationの追加．fullNameやinstanceNameの自動割り当ても行う
 */
Value ProcessStore::addOperation(const std::shared_ptr<Operation>& operation) {
  auto temp = std::dynamic_pointer_cast<OperationBase>(operation);
  return add<OperationBase>(operations_, temp, ".ope");
}

/**
 * Operationの追加．fullNameやinstanceNameの自動割り当ても行う
 */
Value ProcessStore::addOperation(std::shared_ptr<Operation>&& operation) {
  return add<OperationBase>(operations_, std::dynamic_pointer_cast<OperationBase>(operation), ".ope");
}

/**
 * Operationの取得
 */
std::shared_ptr<OperationBase> ProcessStore::getOperation(const std::string& fullName) {
  return get<OperationBase>(operations_, fullName, nullOperation);
}

std::shared_ptr<OperationBase> ProcessStore::getAllOperation(const std::string& fullName) {
  auto op = getOperation(fullName);
  if (op->isNull()) {
    return getContainerOperation(fullName);
  }
  return op;
}

/**
 * ExecutionContextの追加．fullNameやinstanceNameの自動割り当ても行う
 */
Value ProcessStore::addExecutionContext(const std::shared_ptr<ExecutionContext>& ec) {
  return add<ExecutionContext>(executionContexts_, ec, ".ec");
}

/**
 * ExecutionContextの取得
 */
std::shared_ptr<ExecutionContext> ProcessStore::getExecutionContext(const std::string& fullName) {
  return get<ExecutionContext>(executionContexts_, fullName, nullExecutionContext);
}

/**
 * Brokerの追加．fullNameやinstanceNameの自動割り当ても行う
 */
Value ProcessStore::addBroker(const std::shared_ptr<Broker>& brk) {
  return add<Broker>(brokers_, brk, ".brk");
}

/**
 * Brokerの追加．fullNameやinstanceNameの自動割り当ても行う
 */
Value ProcessStore::addBroker(std::shared_ptr<Broker>&& brk) {
  return add<Broker>(brokers_, std::move(brk), ".brk");
}

/**
 * Brokerの取得
 */
std::shared_ptr<Broker> ProcessStore::getBroker(const std::string& fullName) {
  return get<Broker>(brokers_, fullName, nullBroker);
}

/*
Value ProcessStore::addContainer(std::shared_ptr<ContainerBase> container) {
  trace("Process::addContainer({})", container->info());
  if (container->isNull()) {
    return Value::error(logger::error("Process::addContainer failed. Container is null."));
  }
  if (container->getInstanceName() == "") {
    auto nameSpace = "";
    auto name = nerikiri::numbering_policy<std::shared_ptr<ContainerBase>>(containers_, container->info().at("typeName").stringValue(), ".ctn");
    container->setFullName(nameSpace, name);
  } else if (!getContainer(container->info())->isNull()) {
    return Value::error(logger::error("Process::addOperation({}) Error. Process already has the same name operation", container->info().at("name").stringValue()));
  }
  containers_.push_back(container);
  return container->info();
}
*/

/*
std::shared_ptr<ContainerBase> ProcessStore::getContainer(const Value& info) {
  for(auto& c : containers_) {
    if (c->info().at("fullName") == info.at("fullName")) return c;
  }
  return nullContainer();
}
*/

Value ProcessStore::getOperationFactoryInfos() {
  return nerikiri::map<Value, std::shared_ptr<OperationFactory>>(operationFactories_, [](auto& opf) { return Value(opf->typeName()); });
}

Value ProcessStore::getContainerFactoryInfos() {
  return nerikiri::map<Value, std::shared_ptr<ContainerFactoryBase>>(containerFactories_, [](auto& opf) { return Value(opf->typeName()); });
}

/*
Value ProcessStore::addOperation(std::shared_ptr<Operation> op) {
  logger::trace("Process::addOperation({})", op->info());
  if (op->isNull()) {
    return Value::error(logger::error("Process::addOperation failed. Operation is null."));
  }
  auto nameSpace = "";
  if (op->getInstanceName() == "") {
    auto name = nerikiri::numbering_policy<std::shared_ptr<Operation>>(operations_, op->info().at("typeName").stringValue(), ".ope");
    op->setFullName(nameSpace, name);
  } else if (!getOperation(op->info())->isNull()) {
    return Value::error(logger::error("Process::addOperation({}) Error. Process already has the same name operation", op->info().at("typeName").stringValue()));
  } else {
    op->setFullName(nameSpace, op->getInstanceName());
  }
  operations_.push_back(op);
  return op->info();
}
*/


std::shared_ptr<OperationBase> ProcessStore::getContainerOperation(const std::string& fullName) {
  auto [containerName, operationName] = splitContainerAndOperationName(fullName);
  return getContainer(containerName)->getOperation(operationName);
}

/*
std::shared_ptr<OperationBase> ProcessStore::getOperation(const Value& oi) {
  if (oi.isError()) return nullOperation();
  auto name = oi.at("fullName");
  if (name.isError()) {
    // クエリがfullNameを持ってないとき．
    return nullOperation();
  }
  for(auto& op : operations_) {
    if (op->info().at("fullName") == name) return op;
  }
  return this->getContainerOperation(oi);
}
*/

/*
Value ProcessStore::addExecutionContext(std::shared_ptr<ExecutionContext> ec) {
  if (!ec) {
    return Value::error(logger::error("Process::addExecutionContext failed. Execution Context is null"));
  }
  logger::trace("Process::addExecutionContext({})", ec->info());
  if (ec->getInstanceName() == "") {
    auto nameSpace = "";
    auto name = nerikiri::numbering_policy<std::shared_ptr<ExecutionContext>>(executionContexts_, ec->info().at("name").stringValue(), ".ec");
    ec->setFullName(nameSpace, name);
  } else if (!getExecutionContext(ec->info())->isNull()) {
     return Value::error(logger::error("Process::addExecutionContext({}) Error. Process already has the same name ec", ec->info().at("name").stringValue()));
  }
  executionContexts_.push_back(ec);
  return ec->info();
}
*/

/*
std::shared_ptr<ExecutionContext> ProcessStore::getExecutionContext(const Value& info) {
  for(auto& ec : executionContexts_) {
    if (ec->info().at("instanceName") == info.at("instanceName")) {
      return ec;
    }
  }
  return ExecutionContext::null;
}
*/


std::shared_ptr<OperationBase> ProcessStore::getOperationOrTopic(const std::string& fullName) {
  auto op = getAllOperation(fullName);
  if (!op->isNull()) {
    return op;
  }
  return getTopic(fullName);
}


std::shared_ptr<OperationFactory> ProcessStore::getOperationFactory(const Value& oi) {
  for(auto& opf : operationFactories_) {
    if (opf->typeName() == oi.at("typeName").stringValue()) {
      return opf;
    }
  }
  return nullptr;
}

Value ProcessStore::getOperationInfos() {
  auto ops = nerikiri::map<Value, std::shared_ptr<OperationBase>>(operations_, [](auto& op) { return op->info();});
  /*
  for(auto& c : this->containers_) {
    auto infos = c->getOperationInfos();
    ops.insert(ops.end(), infos.begin(), infos.end());
  }*/
  return ops;
}

Value ProcessStore::getAllOperationInfos() {
  auto ops = nerikiri::map<Value, std::shared_ptr<OperationBase>>(operations_, [](auto& op) { return op->info();});
  for(auto& c : this->containers_) {
    auto infos = c->getOperationInfos();
    ops.insert(ops.end(), infos.begin(), infos.end());
  }
  return ops;
}



Value ProcessStore::getConnectionInfos() const {
  auto ocons = nerikiri::map<Value, std::shared_ptr<OperationBase>>(operations_, [](auto& op) -> Value {
    return op->getOutputConnectionInfos();
  });

  auto ccons = nerikiri::map<Value, std::shared_ptr<ContainerBase>>(containers_, [](auto& c) -> Value {
    auto lcons = nerikiri::map<Value, std::shared_ptr<ContainerOperationBase>>(c->getOperations(), [](auto& co) -> Value {
      return co->getOutputConnectionInfos();
    });
    return nerikiri::lift(lcons);
  });

  return nerikiri::merge(nerikiri::lift(ocons), nerikiri::lift(ccons));
}

ProcessStore& ProcessStore::addExecutionContextFactory(std::shared_ptr<ExecutionContextFactory> ec) {
  executionContextFactories_.push_back(ec);
  return *this;
}

ProcessStore& ProcessStore::addOperationFactory(std::shared_ptr<OperationFactory> f) {
  operationFactories_.push_back(f);
  return *this;
}

ProcessStore& ProcessStore::addContainerFactory(std::shared_ptr<ContainerFactoryBase> f) {
  containerFactories_.push_back(f);
  return *this;
}

Value ProcessStore::addContainerOperationFactory(std::shared_ptr<ContainerOperationFactoryBase> cof) {
  auto name = cof->containerTypeName();
  for(auto& cf : containerFactories_) {
    if(cf->typeName() == cof->containerTypeName()) {
      cf->addOperationFactory(cof);
      return {{"typeName", cf->typeName()}};
    }
  }
  return Value::error(logger::error("ProcessStore::addContainerOperationFactory({}) can not find appropreate Container Factory.", cof->typeName()));
}


std::shared_ptr<ContainerFactoryBase> ProcessStore::getContainerFactory(const Value& info) {
  for(auto f : containerFactories_) {
      auto tn = f->typeName();
    if (tn == info.at("typeName").stringValue()) {
      return f;
    }
  }
  logger::error("ProcessStore::getContainerFactory({}) failed. Can not find appropreate container factory.", info.at("typeName"));
  return nullptr;
}

std::shared_ptr<ExecutionContextFactory> ProcessStore::getExecutionContextFactory(const Value& info) {
  for(auto f : executionContextFactories_) {
    if (f->typeName() == info.at("typeName").stringValue()) {
      return f;
    }
  }
  logger::error("getExecutionContext failed. Can not find appropreate execution context factory.");
  return nullptr;
}


Value ProcessStore::getExecutionContextInfos() {
  return nerikiri::map<Value, std::shared_ptr<ExecutionContext>>(executionContexts_, [](auto& ec) { return Value(ec->info()); });
}

Value ProcessStore::getExecutionContextFactoryInfos() {
  return nerikiri::map<Value, std::shared_ptr<ExecutionContextFactory>>(executionContextFactories_, [](auto& ecf) { return Value(ecf->typeName()); });
}


Value ProcessStore::addBrokerFactory(std::shared_ptr<BrokerFactory> factory) {
  auto name = factory->typeName();
  brokerFactories_.push_back(factory);
  return {{"typeName", factory->typeName()}};
}


/*
Value ProcessStore::addBroker(const std::shared_ptr<Broker> brk) {
  logger::trace("ProcessStore::addBroker()");
  if (brk && !brk->isNull()) {
    auto name = nerikiri::numbering_policy<std::shared_ptr<Broker>>(brokers_, brk->info().at("name").stringValue(), ".brk");
    brk->setInstanceName(name);
    brokers_.push_back(brk);
    return brk->info();
  } 
  return Value::error(logger::error("addBroker failed. Added broker is null."));
}
*/

/*
std::shared_ptr<Broker> ProcessStore::getBroker(const Value& info) {
  for(auto& brk : brokers_) {
    if (brk->info().at("instanceName") == info.at("instanceName")) {
      return brk;
    }
  }
  return Broker::null;
}
*/

Value ProcessStore::getBrokerInfos() const {
  return nerikiri::map<Value, std::shared_ptr<Broker>>(brokers_, [](auto& brk) { return brk->info();});
}


std::shared_ptr<BrokerFactory> ProcessStore::getBrokerFactory(const Value& info) {
  for(auto f : brokerFactories_) {
    if(f->typeName() == info.at("typeName").stringValue()) {
      return f;
    }
  }
  return std::make_shared<BrokerFactory>(); // Null Broker Factory
}

Value ProcessStore::addDLLProxy(std::shared_ptr<DLLProxy> dllproxy) {
  dllproxies_.push_back(dllproxy);
  return Value{{"STATUS", "OK"}};
}

Value ProcessStore::getCallbacks() const {
  return process_->getCallbacks();
}

std::shared_ptr<TopicFactory> ProcessStore::getTopicFactory(const Value& topicInfo) {
  for(auto& tf : topicFactories_) {
    if (tf->info().at("name") == topicInfo.at("name")) {
      return tf;
    }
  }
  return std::make_shared<NullTopicFactory>();
}

Value ProcessStore::addTopicFactory(std::shared_ptr<TopicFactory> tf) {
  if (tf == nullptr) {
    return Value::error(logger::error("ProcessStore::addTopicFactory() failed. TopicFactory is null"));
  }
  if (getTopicFactory(tf->info()) == nullptr) {
    topicFactories_.push_back(tf);
  }
  return tf->info();
}

std::shared_ptr<Topic> ProcessStore::getTopic(const Value& topicInfo) {
  for(auto& t : topics_) {
    if (t->info().at("fullName") == topicInfo.at("fullName")) {
      return t;
    }
  }
  return nullTopic();
}

Value ProcessStore::getTopicInfos() const {
  return nerikiri::map<Value, std::shared_ptr<Topic>>(topics_, [](auto t) {
    return t->info();
  });
}

Value ProcessStore::addTopic(std::shared_ptr<Topic> topic) {
  if (topic == nullptr) {
    return Value::error(logger::error("ProcessStore::addTopic() failed. Topic is null"));
  }
  if (getTopic(topic->info())->isNull()) {
    topics_.push_back(topic);
  }
  return topic->info();
}



Value ProcessStore::deleteOperation(const Value& info) {
  
}

Value ProcessStore::deleteContainer(const Value& info) {

}

Value ProcessStore::deleteExecutionContext(const Value& info) {

}


