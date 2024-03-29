
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


/**
 * コンストラクタ
 * 
 * 名前はデフォルトの"store"になります
 */
ProcessStore::ProcessStore(ProcessAPI* process) : ProcessStoreAPI("store"), process_(process) {
  // std::cout << "ProcessStore::ProcessStore(process) called." << std::endl;
  logger::info("ProcesStore::ProcessStore(process) called.");
}


ProcessStore::ProcessStore(ProcessAPI* process, const std::string& name) : ProcessStoreAPI(name), process_(process) {
  // std::cout << "ProcessStore::ProcessStore(process, name=" << name << ") called." << std::endl;
  logger::info("ProcesStore::ProcessStore(process, name={}) called.", name);
}


ProcessStore::~ProcessStore() {
  /// クリアする順序が大事．他のオブジェクトへの直接のポインタを保持しているECなどは先に削除する必要がある
  logger::info("ProcessStore::~ProcessStore() called");
  logger::info("deleting containers...");
  auto containers = list<ContainerAPI>();
  std::for_each(containers.begin(), containers.end(), [this](auto c) {
    this->del<ContainerAPI>(c->fullName());
    logger::info("deleting ContainerAPI({})", c->fullName());
    c->finalize();
  });

  logger::info("deleting operations...");
  auto operations = list<OperationAPI>();
  std::for_each(operations.begin(), operations.end(), [this](auto c) {
    this->del<OperationAPI>(c->fullName());
    logger::info("deleting OperationAPI({})", c->fullName());
    c->finalize();
  });

  containers.clear();
  executionContextFactories_.clear();
  brokers_.clear();
  brokerFactories_.clear();
  logger::info("ProcessStore::~ProcessStore() exit");
}

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
  logger::error("Currently registered ExecutionContextFactoryAPI objects are:");
  auto ecfs = executionContextFactories();
  std::for_each(ecfs.begin(), ecfs.end(), [](auto ecf) {
    logger::error("   - {}", ecf->typeName());
  });
  logger::error(" ---- ");
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

/*
std::shared_ptr<ExecutionContextAPI> ProcessStore::executionContextProxy(const Value& fullName) const {
  return 
}
*/

std::shared_ptr<OperationAPI> ProcessStore::operationProxy(const Value& info) {
  logger::trace("ProcessStore::operationProxy({}) called", info);
  if (info.hasKey("fullName")) {
    auto fullName = Value::string(info["fullName"]);
    auto f = juiz::functional::find<std::shared_ptr<OperationAPI>>(operationProxies(), [&fullName](auto b) { return b->fullName() == fullName; });
    if (f) {
      return f.value();
    }
    if (info.hasKey("broker")) {
      logger::trace("ProcessStore::operationProxy({}) called. Creating OperationProxy....", info);
      return ProxyBuilder::operationProxy(info, this);
    }
  } else if (info.isStringValue()) {
    const auto fullName = info.stringValue();
    auto f = juiz::functional::find<std::shared_ptr<OperationAPI>>(operationProxies(), [&fullName](auto b) { return b->fullName() == fullName; });
    if (f) {
      return f.value();
    }
    return ProxyBuilder::operationProxy(info, this);
    
  }
  return get<OperationAPI>(Value::string(info.at("fullName")));
}

std::shared_ptr<InletAPI> ProcessStore::inletProxy(const Value& info) {
  logger::trace("ProcessStore::inletProxy({}) called", info);
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
  } else if (info.isStringValue()) {
    /// Full path of inlet
    const std::string fullPath = getStringValue(info, "");
    const auto separator_pos = fullPath.rfind(":");
    if (separator_pos == std::string::npos) {
      logger::error("ProcessStore::inletProxy({}) failed. passed argument is wrong.", info);
      return nullObject<InletAPI>();
    }
    const std::string operationFullPath = fullPath.substr(0, separator_pos);
    logger::trace("operationFullPath is {}", operationFullPath);
    const std::string inletName = fullPath.substr(separator_pos+1);
    const auto operationProxy = this->operationProxy(operationFullPath);
    if (operationProxy->isNull()) {
      logger::error("ProcessStore::inletProxy({}) failed. Created OperationProxy(info={}) is null.", info);
    }
    auto ip = operationProxy->inlet(inletName);
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
  logger::trace("ProcessStore::outletProxy({}) called", info);
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
  } else if (info.isStringValue()) {
    /// Full path of inlet
    const std::string fullPath = getStringValue(info, "");
    const std::string operationFullPath = fullPath;
    const auto operationProxy = this->operationProxy(operationFullPath);
    if (operationProxy->isNull()) {
      logger::error("ProcessStore::inletProxy({}) failed. Created OperationProxy() is null.", info);
    }
    auto op = operationProxy->outlet();
    addOutletProxy(op);
    return op;
  }
  
  
  /* else if (info.hasKey("fsm")) { /// もしfsm側のinletならば
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
  if (!dllproxy) {
    return Value(logger::error("ProcessStore::addDLLProxy failed. dllproxy is nullptr."));
  }
  dllproxies_.push_back(dllproxy);
  return Value{{"STATUS", "OK"}};
}

Value ProcessStore::getCallbacks() const {
  return ((ProcessImpl*)process_)->getCallbacks();
}
