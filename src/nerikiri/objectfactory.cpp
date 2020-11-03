
#include "nerikiri/logger.h"
#include "nerikiri/abstractbrokerproxy.h"
#include "nerikiri/nullbrokerproxy.h"
#include "nerikiri/objectfactory.h"



using namespace nerikiri;



Value ObjectFactory::createOperation(ProcessStore& store, const Value& info) {
  logger::trace("Process::createOperation({})", (info));
  auto fullName = Value::string(info.at("fullName"));
  return store.addOperation(store.operationFactory(Value::string(info.at("typeName")))->create(fullName));
}

Value ObjectFactory::createContainer(ProcessStore& store, const Value& info) {
  logger::trace("Process::createContainer({})", (info));
  auto c = store.containerFactory(Value::string(info.at("typeName")))->create(Value::string(info.at("fullName")));
  info.at("operations").const_list_for_each([&store, &c](auto& value) {
    store.addOperation(store.containerOperationFactory(c->typeName(), Value::string(value.at("typeName")))->create(c, Value::string(value.at("fullName"))));
  });
  return store.addContainer(c);
}

Value ObjectFactory::createContainerOperation(ProcessStore& store, const std::string& containerFullName, const Value& info) {
  logger::trace("Process::createContainerOperation({}, {})", containerFullName, info);
  auto fullName = Value::string(info.at("fullName)"));
  return store.addOperation(store.containerOperationFactory(Value::string(info.at("typeName")))->create(store.container(containerFullName), fullName));
}


Value ObjectFactory::createBroker(ProcessStore& store, const Value& ci) {
  logger::trace("Process::createBroker({})", (ci));

  return store.addBroker(store.getBrokerFactory(ci)->create(ci));
}

std::shared_ptr<BrokerAPI>  ObjectFactory::createBrokerProxy(ProcessStore& store, const Value& bi) {
  logger::trace("ObjectFactory::createBrokerProxy({})", (bi));
  return store.getBrokerFactory(bi)->createProxy(bi);
}

Value ObjectFactory::createExecutionContext(ProcessStore& store, const Value& value) {
  logger::debug("ObjectFactory::createExecutionContext({})", value);
  return store.addEC(store.executionContextFactory(Value::string(value.at("typeName")))->create(Value::string(value.at("fullName"))));
}

Value ObjectFactory::createTopic(ProcessStore& store, const Value& topicInfo) {
  logger::debug("ObjectFactory::createTopic({})", topicInfo);
  return store.addTopic(store.topicFactory(Value::string("typeName"))->create(Value::string(topicInfo.at("fullName"))));
}

Value ObjectFactory::createFSM(ProcessStore& store, const Value& fsmInfo) {
  logger::debug("ObjectFactory::createFSM({})", fsmInfo);
  return store.addFSM(store.fsmFactory(Value::string(fsmInfo.at("typeName")))->create(Value::string(fsmInfo.at("fullName"))));
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