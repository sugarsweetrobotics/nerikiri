
#include <sstream>
#include "nerikiri/nerikiri.h"
#include "nerikiri/functional.h"
#include "nerikiri/logger.h"
#include "nerikiri/signal.h"
#include "nerikiri/naming.h"

#include "process_impl.h"

#include "nerikiri/broker_api.h"
#include <nerikiri/proxy_builder.h>
#include <iostream>

using namespace nerikiri;
using namespace nerikiri::logger;


Value ProcessStore::info() const { return process_->info(); }

Value ProcessStore::addBroker(const std::shared_ptr<BrokerAPI>& b) {
  return add<BrokerAPI>(brokers_, b, ".brk");
}

Value ProcessStore::deleteBroker(const std::string& fullName) {
  return del<BrokerAPI>(brokers_, fullName);
}

Value ProcessStore::addBrokerFactory(const std::shared_ptr<BrokerFactoryAPI>& bf) {
  return add<BrokerFactoryAPI>(brokerFactories_, bf, ".bf");
}

Value ProcessStore::deleteBrokerFactory(const std::string& fullName) {
  return del<BrokerFactoryAPI>(brokerFactories_, fullName);
}



std::shared_ptr<OperationAPI> ProcessStore::operation(const std::string& fullName) const { 
  auto op = nerikiri::functional::find<std::shared_ptr<OperationAPI>>(operations(), [&fullName](auto op) { return op->fullName() == fullName; });
  if (op) return op.value();;
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, fullName);
  return nullOperation();
}

std::shared_ptr<OperationFactoryAPI> ProcessStore::operationFactory(const std::string& operationTypeFullName) const {
  auto f = nerikiri::functional::find<std::shared_ptr<OperationFactoryAPI>>(operationFactories(), [&operationTypeFullName](auto f) { return f->typeName() == operationTypeFullName; });
  if (f) return f.value();
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, operationTypeFullName);
  return nullOperationFactory();
}

std::shared_ptr<ContainerAPI> ProcessStore::container(const std::string& fullName) const { 
  auto op = nerikiri::functional::find<std::shared_ptr<ContainerAPI>>(containers(), [&fullName](auto op) { 
    return op->fullName() == fullName; 
  });
  if (op) return op.value();;
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, fullName);

  return nullContainer();
}

std::shared_ptr<ContainerFactoryAPI> ProcessStore::containerFactory(const std::string& containerTypeFullName) const {
  auto f = nerikiri::functional::find<std::shared_ptr<ContainerFactoryAPI>>(containerFactories(), [&containerTypeFullName] (auto f) {
    return f->typeName() == containerTypeFullName;
  });
  if (f) return f.value();
  
  logger::error("ProcessStore::{}({}, {}) called, but not found.", __func__, containerTypeFullName);

  return nullContainerFactory();
}

std::shared_ptr<ContainerOperationFactoryAPI> ProcessStore::containerOperationFactory(const std::string& containerOperationTypeFullName) const {
  const auto& [containerTypeFullName, operationTypeFullName] = nerikiri::naming::splitContainerAndOperationName(containerOperationTypeFullName);
  return containerOperationFactory(containerTypeFullName, operationTypeFullName);
}

std::shared_ptr<ContainerOperationFactoryAPI> ProcessStore::containerOperationFactory(const std::string& containerTypeFullName, const std::string& operationTypeFullName) const {
  auto f = nerikiri::functional::find<std::shared_ptr<ContainerOperationFactoryAPI>>(containerOperationFactories(), [&containerTypeFullName, &operationTypeFullName] (auto f) {
    return f->containerTypeFullName() == containerTypeFullName && f->operationTypeFullName() == operationTypeFullName;
  });
  if (f) return f.value();
  logger::error("ProcessStore::{}({}, {}) called, but not found.", __func__, containerTypeFullName, operationTypeFullName);
  return nullContainerOperationFactory();
}

std::shared_ptr<FSMAPI> ProcessStore::fsm(const std::string& fullName) const {
  auto f = nerikiri::functional::find<std::shared_ptr<FSMAPI>>(fsms(), [&fullName](auto fsm) { return fsm->fullName() == fullName; });
  if (f) return f.value();;
  
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, fullName);
  return nullFSM();
}

std::shared_ptr<FSMFactoryAPI> ProcessStore::fsmFactory(const std::string& fsmTypeFullName) const {
  auto f = nerikiri::functional::find<std::shared_ptr<FSMFactoryAPI>>(fsmFactories(), [&fsmTypeFullName] (auto f) {
    return f->typeName() == fsmTypeFullName;
  });
  if (f) return f.value();
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, fsmTypeFullName);
  return nullFSMFactory();
}

std::shared_ptr<TopicAPI> ProcessStore::topic(const std::string& fullName) const {
  auto f = nerikiri::functional::find<std::shared_ptr<TopicAPI>>(topics(), [&fullName](auto t) { return t->fullName() == fullName; });
  if (f) return f.value();;
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, fullName);
  return nullTopic();
}

std::shared_ptr<TopicFactoryAPI> ProcessStore::topicFactory(const std::string& topicTypeFullName) const {
  auto f = nerikiri::functional::find<std::shared_ptr<TopicFactoryAPI>>(topicFactories(), [&topicTypeFullName] (auto f) {
    return f->topicTypeFullName() == topicTypeFullName;
  });
  if (f) return f.value();
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, topicTypeFullName);
  return nullTopicFactory();
}

std::shared_ptr<ExecutionContextAPI> ProcessStore::executionContext(const std::string& fullName) const {
  auto f = nerikiri::functional::find<std::shared_ptr<ExecutionContextAPI>>(executionContexts(), [&fullName](auto ec) { return ec->fullName() == fullName; });
  if (f) return f.value();;
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, fullName);
  return nullEC();
}

std::shared_ptr<ExecutionContextFactoryAPI> ProcessStore::executionContextFactory(const std::string& ecTypeFullName) const {
  auto f = nerikiri::functional::find<std::shared_ptr<ExecutionContextFactoryAPI>>(executionContextFactories(), [&ecTypeFullName] (auto f) {
    return f->executionContextTypeFullName() == ecTypeFullName;
  });
  if (f) return f.value();
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, ecTypeFullName);
  return nullECFactory();
}

std::shared_ptr<BrokerAPI> ProcessStore::broker(const std::string& fullName) const {
  auto f = nerikiri::functional::find<std::shared_ptr<BrokerAPI>>(brokers(), [&fullName](auto b) { return b->fullName() == fullName; });
  if (f) return f.value();;
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, fullName);
  return std::make_shared<NullBroker>();
}

std::shared_ptr<BrokerFactoryAPI> ProcessStore::brokerFactory(const std::string& fullName) const {
  auto f = nerikiri::functional::find<std::shared_ptr<BrokerFactoryAPI>>(brokerFactories(), [&fullName](auto f) { return f->typeName() == fullName; });
  if (f) return f.value();;
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, fullName);
  return nullBrokerFactory();
}

std::shared_ptr<OperationAPI> ProcessStore::operationProxy(const Value& info) {
  if (info.hasKey("broker")) {
    return ProxyBuilder::operationProxy(info, this);
  }
  return operation(Value::string(info.at("fullName")));
}


/**
 * ExecutionContextの追加．fullNameやinstanceNameの自動割り当ても行う
 */
//Value ProcessStore::addExecutionContext(const std::shared_ptr<ExecutionContext>& ec) {
//  return add<ExecutionContext>(executionContexts_, ec, ".ec");
//}

/**
 * ExecutionContextの取得
 */
//std::shared_ptr<ExecutionContext> ProcessStore::getExecutionContext(const std::string& fullName) {
//  return get<ExecutionContext>(executionContexts_, fullName, nullExecutionContext);//
//}

/**
 * Brokerの追加．fullNameやinstanceNameの自動割り当ても行う
 *
Value ProcessStore::addBroker(const std::shared_ptr<BrokerAPI>& brk) {
  return add<Broker>(brokers_, brk, ".brk");
}
*/

/**
 * Brokerの追加．fullNameやinstanceNameの自動割り当ても行う
 *
Value ProcessStore::addBroker(std::shared_ptr<BrokerAPI>&& brk) {
  return add<Broker>(brokers_, std::move(brk), ".brk");
}
*/

/**
 * Brokerの取得
 *
std::shared_ptr<Broker> ProcessStore::getBroker(const std::string& fullName) {
  return get<Broker>(brokers_, fullName, nullBroker);
}
*/

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

/*
std::shared_ptr<OperationBase> ProcessStore::getContainerOperation(const std::string& fullName) {
  auto [containerName, operationName] = splitContainerAndOperationName(fullName);
  return getContainer(containerName)->getOperation(operationName);
}
*/

/*
std::shared_ptr<OperationBase> ProcessStore::getOperationOrTopic(const std::string& fullName) {
  auto op = getAllOperation(fullName);
  if (!op->isNull()) {
    return op;
  }
  return getTopic(fullName);
}

*


Value ProcessStore::addContainerOperationFactory(const std::shared_ptr<ContainerOperationFactoryBase>& cof) {
  auto name = cof->containerTypeName();
  for(auto& cf : containerFactories_) {
    if(cf->typeName() == cof->containerTypeName()) {
      cf->addOperationFactory(cof);
      return {{"typeName", cf->typeName()}};
    }
  }
  return Value::error(logger::error("ProcessStore::addContainerOperationFactory({}) can not find appropreate Container Factory.", cof->typeName()));
}
*/

/*


Value ProcessStore::addBrokerFactory(const std::shared_ptr<BrokerFactory>& factory) {
  auto name = factory->typeName();
  brokerFactories_.push_back(factory);
  return {{"typeName", factory->typeName()}};
}
*/

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
/*
Value ProcessStore::getBrokerInfos() const {
  return nerikiri::map<Value, std::shared_ptr<Broker>>(brokers_, [](auto& brk) { return brk->info();});
}
*/
/*

std::shared_ptr<BrokerFactory> ProcessStore::getBrokerFactory(const Value& info) {
  for(auto f : brokerFactories_) {
    if(f->typeName() == info.at("typeName").stringValue()) {
      return f;
    }
  }
  return std::make_shared<BrokerFactory>(); // Null Broker Factory
}
*/

Value ProcessStore::addDLLProxy(const std::shared_ptr<DLLProxy>& dllproxy) {
  dllproxies_.push_back(dllproxy);
  return Value{{"STATUS", "OK"}};
}

Value ProcessStore::getCallbacks() const {
  return process_->getCallbacks();
}

/*
std::shared_ptr<TopicFactory> ProcessStore::getTopicFactory(const Value& topicInfo) {
  logger::trace("ProcessStore::getTopicFactory({}) called", topicInfo);
  std::string typeName = "TopicFactory";
  if (topicInfo.hasKey("typeName")) typeName = topicInfo.at("typeName").stringValue();
  for(auto& tf : topicFactories_) {
    if (tf->info().at("typeName").stringValue() == typeName) {
      return tf;
    }
  }
  logger::error("ProcessStore::getTopicFactory failed. TopicInfo({}) was not found.", topicInfo);
  return std::make_shared<NullTopicFactory>();
}
*/
/*
Value ProcessStore::addTopicFactory(const std::shared_ptr<TopicFactory>& tf) {
  if (tf == nullptr) {
    return Value::error(logger::error("ProcessStore::addTopicFactory() failed. TopicFactory is null"));
  }
  if (getTopicFactory(tf->info())->isNull()) {
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
  logger::error("ProcessStore::getTopic({}) failed. Can not be found", topicInfo);
  return nullTopic();
}

std::shared_ptr<Topic> ProcessStore::getTopic(const std::string& fullName) {
  return get<Topic>(topics_, fullName, nullTopic);
}

Value ProcessStore::getTopicInfos() const {
  return nerikiri::map<Value, std::shared_ptr<Topic>>(topics_, [](auto t) {
    return t->info();
  });
}

Value ProcessStore::addTopic(const std::shared_ptr<Topic>& topic) {
  logger::debug("ProcessStore::addTopic({}) called.", topic->info());
  if (topic == nullptr) {
    return Value::error(logger::error("ProcessStore::addTopic({}) failed. Topic is null", topic->info()));
  }
  if (getTopic(topic->info().at("fullName").stringValue())->isNull()) {
    topics_.push_back(topic);
  }
  logger::info("ProcessStore::addTopic({}) success.", topic->info());
  return topic->info();
}

Value ProcessStore::addFSM(const std::shared_ptr<FSM>& fsm) {
  return this->add<FSM>(fsms_, fsm, ".fsm");
}

std::shared_ptr<FSM> ProcessStore::getFSM(const std::string& fullName) {
  return get<FSM>(fsms_, fullName, nullFSM);
}

Value ProcessStore::addFSMFactory(const std::shared_ptr<FSMFactory>& ff) {
  if (ff == nullptr) {
    return Value::error(logger::error("ProcessStore::addFSMFactory() failed. FSMFactory is null"));
  }
  if (ff->isNull()) {
    return Value::error(logger::error("ProcessStore::addFSMFactory() failed. FSMFactory is null"));
  }
  if (getFSMFactory(ff->info())->isNull()) {
    fsmFactories_.push_back(ff);
  }
  return ff->info();
}

Value ProcessStore::getFSMInfos() const {
  return nerikiri::map<Value, std::shared_ptr<FSM>>(fsms_, [](auto t) {
    return t->info();
  });
}

std::shared_ptr<FSMFactory> ProcessStore::getFSMFactory(const Value& fsmInfo) {
  for(auto& f : fsmFactories_) {
    if (f->info().at("typeName") == fsmInfo.at("typeName")) {
      return f;
    }
  }
  return std::make_shared<NullFSMFactory>();
}
*/

/*
std::shared_ptr<OperationAPI> ProcessStore::operationProxy(const Value& info) {
  if (info.hasKey("broker")) {
    auto f = brokerFactory(Value::string(info.at("broker").at("typeName")));
    if (!f) {
      logger::error("ProcessStore::operationProxy({}) failed. Broker (typeName={}) can not be found", info, Value::string(info.at("broker").at("typeName")));
      return std::make_shared<NullOperation>();
    }
    return std::make_shared<OperationProxy>(f->createProxy(info.at("broker")));
  }

  auto op = operation(info.at("fullName"));
  if (op) return op.value();
  return std::make_shared<NullOperation>();
}

*/