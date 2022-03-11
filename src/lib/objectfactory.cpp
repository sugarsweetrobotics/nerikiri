
#include <juiz/logger.h>
#include "./objectfactory.h"


#include "./fsm/fsm_container.h"
#include "./ec/ec_container.h"
#include "./anchor/anchor_container.h"


using namespace juiz;

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
  if (info.hasKey("instanceName")) return info["instanceName"].stringValue();
  if (info.hasKey("fullName")) return info["fullName"].stringValue();
  if (info.hasKey("typeName")) return info["typeName"].stringValue() + numberingPolicy(ts, info);;
  return "object";
}


Value ObjectFactory::createOperation(ProcessStore& store, const Value& info) {
  logger::trace("ObjectFactory::createOperation({}) called", (info));
  return store.add<OperationAPI>(store.get<OperationFactoryAPI>(info["typeName"])->create<OperationAPI>(loadFullName(store.list<OperationAPI>(), info)));
}

Value ObjectFactory::createContainer(ProcessStore& store, const Value& info) {
  logger::trace("ObjectFactory::createContainer({}) called", (info));
  return store.add<ContainerAPI>(store.get<ContainerFactoryAPI>(info["typeName"])->create<ContainerAPI>(loadFullName(store.list<ContainerAPI>(), info)));
}

Value ObjectFactory::createContainerOperation(ProcessStore& store, const Value& cInfo, const Value& info) {
  logger::trace("ObjectFactory::createContainerOperation(store, cinfo={}, info={}) called", cInfo, info);
  auto container = store.get<ContainerAPI>(cInfo["fullName"]);
  auto cof = store.get<ContainerOperationFactoryAPI>(naming::join(Value::string(cInfo["typeName"]), Value::string(info["typeName"])));
  auto cop = cof->create<OperationAPI>(naming::join(Value::string(cInfo["fullName"]), loadFullName(store.list<OperationAPI>(), info)), info);
  cop->setOwner(container);
  return container->addOperation(cop);
}

Value ObjectFactory::createBroker(ProcessStore& store, const Value& info) {
  logger::info("ObjectFactory::createBroker({})", info);
  //auto fullName = Value::string(info.at("fullName)"));
  auto fullName = loadFullName(store.brokers(), info);
  return store.addBroker(store.brokerFactory(Value::string(info.at("typeName")))->create(info));
}

std::shared_ptr<ClientProxyAPI> ObjectFactory::createBrokerProxy(ProcessStore& store, const Value& bi) {
  logger::trace("ObjectFactory::createBrokerProxy({})", bi);
  return store.brokerFactory(Value::string(bi.at("typeName")))->createProxy(Value::string(bi.at("fullName")));
}

Value ObjectFactory::createExecutionContext(ProcessStore& store, const Value& value) {
  logger::info("ObjectFactory::createExecutionContext({})", value);
  // auto info = value;
  //auto fullName = loadFullName(store.executionContexts(), value);
  auto fullName = Value::string(value["fullName"]);
  // info["fullName"] = fullName;
  return juiz::createEC(store, fullName, value);
  // return store.addEC(store.executionContextFactory(Value::string(value.at("typeName")))->create(info));
}

Value ObjectFactory::createAnchor(ProcessStore& store, const Value& value) {
  logger::info("ObjectFactory::createAnchor({})", value);
  // auto info = value;
  //auto fullName = loadFullName(store.executionContexts(), value);
  auto fullName = Value::string(value["fullName"]);
  // info["fullName"] = fullName;
  if(value.hasKey("typeName")) {
    if (value["typeName"].stringValue() == "StaticPeriodicAnchor") {
      return juiz::createStaticPeriodicAnchor(store, fullName, value);
    } else if (value["typeName"].stringValue() == "DynamicPeriodicAnchor") {
      return juiz::createDynamicPeriodicAnchor(store, fullName, value);
    } else {
      return Value::error(logger::error("ObjectFactory::createAnchor failed. Anchor's typeName is invalid({})", value["typeName"]));
    }
  } 
  return juiz::createStaticPeriodicAnchor(store, fullName, value);
  // return store.addEC(store.executionContextFactory(Value::string(value.at("typeName")))->create(info));
}

Value ObjectFactory::createTopic(ProcessStore& store, const Value& topicInfo) {
  logger::info("ObjectFactory::createTopic({})", topicInfo);
  return store.add<TopicAPI>(store.get<TopicFactoryAPI>("Topic")->create(Value::string(topicInfo.at("fullName"))));
}

Value ObjectFactory::createFSM(ProcessStore& store, const Value& fsmInfo) {
  logger::info("ObjectFactory::createFSM({})", fsmInfo);
  // auto fullName = loadFullName(store.fsms(), fsmInfo);
  auto fullName = Value::string(fsmInfo["fullName"]);
  return juiz::createFSM(store, fullName, fsmInfo);
  /*
  auto container = store.containerFactory("_FSMContainerStruct")->create(fullName);
  if (container->isNull()) {
    logger::error("ObjectFactory::createFSM failed");
  }
  */
  // return store.addFSM(store.fsmFactory(Value::string(fsmInfo.at("typeName")))->create(fullName, fsmInfo));
}

Value ObjectFactory::deleteOperation(ProcessStore& store, const std::string& fullName)  {
  // return store.deleteOperation(fullName);
  return store.del<OperationAPI>(fullName);
}

Value ObjectFactory::deleteContainer(ProcessStore& store, const std::string& fullName)  {
  // return store.deleteContainer(fullName);
  return store.del<ContainerAPI>(fullName);
}

Value ObjectFactory::deleteContainerOperation(ProcessStore& store, const std::string& fullName)  {
  //return store.deleteContainerOperation(fullName);
}
        
Value ObjectFactory::deleteExecutionContext(ProcessStore& store, const std::string& fullName) {
  //return store.deleteEC(fullName);
}

//Value ObjectFactory::deleteFSM(ProcessStore& store, const std::string& fullName)  {
//  return store.deleteFSM(fullName);
//}