
#include <juiz/logger.h>
#include "./objectfactory.h"


#include "./fsm/fsm_container.h"
#include "./ec/ec_container.h"


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
  auto fullName = loadFullName(store.list<OperationAPI>(), info);
  logger::info("ObjectFactory is creating an Operation({})", info);
  auto op = store.get<OperationFactoryAPI>(Value::string(info.at("typeName")))->create<OperationAPI>(fullName);
  return store.add<OperationAPI>(op);
}

Value ObjectFactory::createContainer(ProcessStore& store, const Value& info) {
  logger::trace("ObjectFactory::createContainer({}) called", (info));
  auto fullName = loadFullName(store.list<ContainerAPI>(), info);
  logger::info("ObjectFactory is creating a Container({})", info);
  auto c = store.get<ContainerFactoryAPI>(Value::string(info.at("typeName")))->create<ContainerAPI>(fullName);
  auto v = store.add<ContainerAPI>(c);
  if (v.isError()) return v;
  /*
  auto getterInfo = ObjectFactory::createContainerOperation(store, c->info(), {
    {"fullName", "getBasePose.ope"},
    {"typeName", "ContainerGetBasePose"}
  });
  auto setterInfo = ObjectFactory::createContainerOperation(store, c->info(), {
    {"fullName", "setBasePose.ope"},
    {"typeName", "ContainerSetBasePose"}
  });
  */
  return v;
}

Value ObjectFactory::createContainerOperation(ProcessStore& store, const Value& cInfo, const Value& info) {
  logger::trace("ObjectFactory::createContainerOperation({}) called", info);
//  const std::string& containerFullName = Value::string(info.at("containerFullName"));
  auto containerFullName = Value::string(cInfo.at("fullName"));
  //auto fullName = Value::string(info.at("fullName)"));
  auto fullName = nerikiri::naming::join(containerFullName, loadFullName(store.list<OperationAPI>(), info));
  auto typeName = nerikiri::naming::join(Value::string(cInfo.at("typeName")), Value::string(info.at("typeName")));
  auto container = store.get<ContainerAPI>(containerFullName);
  auto cof = store.get<ContainerOperationFactoryAPI>(typeName);
  if (cof->isNull()) {
    return Value::error(logger::error("ObjectFactory::createContainerOperation failed. ContainerOperationFactory({}) is null.", typeName));
  }

  logger::info("ObjectFactory is creating a ContainerOperation({})", info);
  auto cop = cof->create<OperationAPI>(fullName);
  cop->setOwner(container);
  return container->addOperation(cop);

  // return store.addOperation(store.containerOperationFactory(typeName)->create(container, fullName));
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
  return nerikiri::createEC(store, fullName, value);
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