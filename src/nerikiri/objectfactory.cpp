
#include "nerikiri/logger.h"
#include "nerikiri/abstractbrokerproxy.h"
#include "nerikiri/objectfactory.h"


#include "nerikiri/fsm/fsm_container.h"


using namespace nerikiri;

template<class T>
std::string numberingPolicy(const std::vector<T>& ts, const Value& info) {
  if (!info.hasKey("typeName")) return "0";
  int count = 0;
  const auto typeName = info.at("typeName").stringValue();
  for(auto t : ts) {
    if (t->typeName() == typeName) count++;
  }
  return std::to_string(count);
}

template<class T>
std::string loadFullName(const std::vector<T>& ts, const Value& info) {
  if (info.hasKey("instanceName")) return info.at("instanceName").stringValue();
  if (info.hasKey("fullName")) return info.at("fullName").stringValue();
  if (info.hasKey("typeName")) return info.at("typeName").stringValue() + numberingPolicy(ts, info);;
  return "object";
}


Value ObjectFactory::createOperation(ProcessStore& store, const Value& info) {
  logger::trace("ObjectFactory::createOperation({}) called", (info));
  auto fullName = loadFullName(store.operations(), info);
  logger::info("ObjectFactory::createOperation({})", info);
  auto op = store.operationFactory(Value::string(info.at("typeName")))->create(fullName);
  return store.addOperation(op);
}

Value ObjectFactory::createContainer(ProcessStore& store, const Value& info) {
  logger::trace("ObjectFactory::createContainer({})", (info));
  auto fullName = loadFullName(store.containers(), info);
  logger::info("ObjectFactory::createContainer({})", info);
  auto c = store.containerFactory(Value::string(info.at("typeName")))->create(fullName);

  return store.addContainer(c);
}

Value ObjectFactory::createContainerOperation(ProcessStore& store, const Value& cInfo, const Value& info) {
  logger::trace("ObjectFactory::createContainerOperation({})", info);
//  const std::string& containerFullName = Value::string(info.at("containerFullName"));
  auto containerFullName = Value::string(cInfo.at("fullName"));
  //auto fullName = Value::string(info.at("fullName)"));
  auto fullName = nerikiri::naming::join(containerFullName, loadFullName(store.operations(), info));
  auto typeName = nerikiri::naming::join(Value::string(cInfo.at("typeName")), Value::string(info.at("typeName")));
  return store.addOperation(store.containerOperationFactory(typeName)->create(store.container(containerFullName), fullName));
}


Value ObjectFactory::createBroker(ProcessStore& store, const Value& info) {
  logger::info("ObjectFactory::createBroker({})", info);
  //auto fullName = Value::string(info.at("fullName)"));
  auto fullName = loadFullName(store.brokers(), info);
  return store.addBroker(store.brokerFactory(Value::string(info.at("typeName")))->create(info));
}

std::shared_ptr<BrokerProxyAPI> ObjectFactory::createBrokerProxy(ProcessStore& store, const Value& bi) {
  logger::trace("ObjectFactory::createBrokerProxy({})", bi);
  return store.brokerFactory(Value::string(bi.at("typeName")))->createProxy(Value::string(bi.at("fullName")));
}

Value ObjectFactory::createExecutionContext(ProcessStore& store, const Value& value) {
  logger::info("ObjectFactory::createExecutionContext({})", value);
  auto info = value;
  auto fullName = loadFullName(store.executionContexts(), value);
  info["fullName"] = fullName;
  return store.addEC(store.executionContextFactory(Value::string(value.at("typeName")))->create(info));
}

Value ObjectFactory::createTopic(ProcessStore& store, const Value& topicInfo) {
  logger::info("ObjectFactory::createTopic({})", topicInfo);
  return store.addTopic(store.topicFactory(Value::string(topicInfo.at("typeName")))->create(Value::string(topicInfo.at("fullName"))));
}

Value ObjectFactory::createFSM(ProcessStore& store, const Value& fsmInfo) {
  logger::info("ObjectFactory::createFSM({})", fsmInfo);
  auto fullName = loadFullName(store.fsms(), fsmInfo);
  return nerikiri::createFSM(store, fullName, fsmInfo);
  /*
  auto container = store.containerFactory("_FSMContainerStruct")->create(fullName);
  if (container->isNull()) {
    logger::error("ObjectFactory::createFSM failed");
  }
  */
  // return store.addFSM(store.fsmFactory(Value::string(fsmInfo.at("typeName")))->create(fullName, fsmInfo));
}

Value ObjectFactory::deleteOperation(ProcessStore& store, const std::string& fullName)  {
  return store.deleteOperation(fullName);
}

Value ObjectFactory::deleteContainer(ProcessStore& store, const std::string& fullName)  {
  return store.deleteContainer(fullName);
}

Value ObjectFactory::deleteContainerOperation(ProcessStore& store, const std::string& fullName)  {
  return store.deleteContainerOperation(fullName);
}
        
Value ObjectFactory::deleteExecutionContext(ProcessStore& store, const std::string& fullName) {
  return store.deleteEC(fullName);
}

Value ObjectFactory::deleteFSM(ProcessStore& store, const std::string& fullName)  {
  return store.deleteFSM(fullName);
}