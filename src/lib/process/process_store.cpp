
#include <sstream>
#include <juiz/juiz.h>
#include <juiz/utils/functional.h>
#include <juiz/logger.h>
#include <juiz/utils/signal.h>
#include <juiz/utils/naming.h>

#include "process_impl.h"

#include <juiz/broker_api.h>
#include "../proxy_builder.h"
#include <iostream>

using namespace juiz;
using namespace juiz::logger;


Value ProcessStore::info() const { return process_->info(); }


std::vector<std::shared_ptr<ConnectionAPI>> ProcessStore::connections() const {
  std::vector<std::shared_ptr<ConnectionAPI>> cons;
  juiz::functional::for_each<std::shared_ptr<OperationAPI>>(list<OperationAPI>(), [&cons](auto op) {
    auto cs = op->outlet()->connections();
    cons.insert(cons.end(), cs.begin(), cs.end());
  });
  return cons;
}

Value ProcessStore::addBroker(const std::shared_ptr<BrokerAPI>& b) {
  return addObject<BrokerAPI>(brokers_, b, ".brk");
}

Value ProcessStore::deleteBroker(const std::string& fullName) {
  return delObject<BrokerAPI>(brokers_, fullName);
}

Value ProcessStore::addBrokerFactory(const std::shared_ptr<BrokerFactoryAPI>& bf) {
  return addObject<BrokerFactoryAPI>(brokerFactories_, bf, ".bf");
}

Value ProcessStore::deleteBrokerFactory(const std::string& fullName) {
  return delObject<BrokerFactoryAPI>(brokerFactories_, fullName);
}

std::shared_ptr<ConnectionAPI> ProcessStore::connection(const std::string& fullName) const { 
  auto op = juiz::functional::find<std::shared_ptr<ConnectionAPI>>(connections(), [&fullName](auto op) { return op->fullName() == fullName; });
  if (op) return op.value();;
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, fullName);
  return nullConnection();
}

std::shared_ptr<ExecutionContextFactoryAPI> ProcessStore::executionContextFactory(const std::string& ecTypeFullName) const {
  auto f = juiz::functional::find<std::shared_ptr<ExecutionContextFactoryAPI>>(executionContextFactories(), [&ecTypeFullName] (auto f) {
    return f->typeName() == ecTypeFullName;
  });
  if (f) return f.value();
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, ecTypeFullName);
  return nullECFactory();
}

std::shared_ptr<BrokerAPI> ProcessStore::broker(const std::string& fullName) const {
  auto f = juiz::functional::find<std::shared_ptr<BrokerAPI>>(brokers(), [&fullName](auto b) { return b->fullName() == fullName; });
  if (f) return f.value();;
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, fullName);
  return nullBroker();
}

std::shared_ptr<BrokerFactoryAPI> ProcessStore::brokerFactory(const std::string& fullName) const {
  auto f = juiz::functional::find<std::shared_ptr<BrokerFactoryAPI>>(brokerFactories(), [&fullName](auto f) { return f->typeName() == fullName; });
  if (f) return f.value();;
  logger::error("ProcessStore::{}({}) called, but not found.", __func__, fullName);
  return nullBrokerFactory();
}

std::shared_ptr<OperationAPI> ProcessStore::operationProxy(const Value& info) {
  auto fullName = Value::string(info.at("fullName"));
  auto f = juiz::functional::find<std::shared_ptr<OperationAPI>>(operationProxies(), [&fullName](auto b) { return b->fullName() == fullName; });
    if (f) return f.value();
  if (info.hasKey("broker")) {
    logger::trace("ProcessStore::operationProxy({}) called. Creating OperationProxy....", info);
    return ProxyBuilder::operationProxy(info, this);
  }
  return get<OperationAPI>(Value::string(info.at("fullName")));
}

std::shared_ptr<InletAPI> ProcessStore::inletProxy(const Value& info) {
  if (info.hasKey("operation")) { /// もしoperation側のinletならば
    auto p = juiz::functional::find<std::shared_ptr<InletAPI>>(inletProxies(), [&info](auto p) {
      // もしoperationの名前が一緒でnameが一緒のinletproxyがあればそれは同一
      return p->info()["ownerFullName"] == info["operation"]["fullName"] && p->name() == Value::string(info["name"]);
    });
    if (p) return p.value(); // storeにあればそれを使います．
    // なければ作ります．
    logger::trace("ProcessStore::inletProxy({}) called. Creating InletProxy....", info);
    auto operationInfo = info["operation"];
    if (info.hasKey("broker")) {
      operationInfo["broker"] = info["broker"];
    }
    auto operationProxy = this->operationProxy(operationInfo);
    if (operationProxy->isNull()) {
      logger::error("ProcessStore::inletProxy({}) failed. Created OperationProxy(info={}) is null.", info, operationInfo);
    }
    auto ip = operationProxy->inlet(Value::string(info["name"]));
    addInletProxy(ip);
    return ip;
  } 
    
  logger::error("ProcessStore::inletProxy({}) failed. Invalid Info format", info);
  return nullOperationInlet();  
}


std::shared_ptr<ClientProxyAPI> juiz::coreBroker(ProcessStore& store) {
  return store.brokerFactory("CoreBroker")->createProxy("");
}


std::shared_ptr<OutletAPI> ProcessStore::outletProxy(const Value& info) {
  if (info.hasKey("operation")) { /// もしoperation側のinletならば
    auto p = juiz::functional::find<std::shared_ptr<OutletAPI>>(outletProxies(), [&info](auto p) {
      // もしoperationの名前が一緒でnameが一緒のinletproxyがあればそれは同一
      return p->info()["ownerFullName"] == info["operation"]["fullName"];
    });
    if (p) return p.value(); // storeにあればそれを使います．

    // なければ作ります．
    logger::trace("ProcessStore::outletProxy({}) called. Creating OutletProxy....", info);
    auto operationProxy = this->operationProxy(info["operation"]);
    if (operationProxy->isNull()) {
      logger::error("ProcessStore::outletProxy({}) failed. Created OperationProxy(info={}) is null.", info, info["operation"]);
    }
    auto op = operationProxy->outlet();
    addOutletProxy(op);
    return op;
  } /* else if (info.hasKey("fsm")) { /// もしfsm側のinletならば
    auto p = juiz::functional::find<std::shared_ptr<OperationOutletAPI>>(outletProxies(), [&info](auto p) {
      // もしoperationの名前が一緒でnameが一緒のinletproxyがあればそれは同一
      return p->info()["ownerFullName"] == info["fsm"]["fullName"];
    });
    if (p) return p.value(); // storeにあればそれを使います．

    // なければ作ります．
    auto fsmProxy = this->fsmProxy(info["fsm"]);
    auto op = fsmProxy->fsmState(Value::string(info["name"]))->outlet();
    addOutletProxy(op);
    return op;
  }  */

  logger::error("ProcessStore::outletProxy({}) failed. Invalid Info format", info);
  return nullOperationOutlet();
}


std::shared_ptr<ContainerAPI> ProcessStore::containerProxy(const Value& info) {
  auto fullName = Value::string(info.at("fullName"));
  auto f = juiz::functional::find<std::shared_ptr<ContainerAPI>>(containerProxies(), [&fullName](auto b) { return b->fullName() == fullName; });
    if (f) return f.value();
  if (info.hasKey("broker")) {
    logger::trace("ProcessStore::containerProxy({}) called. Creating ContainerProxy....", info);
    return ProxyBuilder::containerProxy(info, this);
  }
  return get<ContainerAPI>(Value::string(info.at("fullName")));
}

Value ProcessStore::addDLLProxy(const std::shared_ptr<DLLProxy>& dllproxy) {
  dllproxies_.push_back(dllproxy);
  return Value{{"STATUS", "OK"}};
}

Value ProcessStore::getCallbacks() const {
  return ((Process*)process_)->getCallbacks();
}
