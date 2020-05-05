
#include "nerikiri/util/logger.h"

#include "nerikiri/objectfactory.h"



using namespace nerikiri;



Value ObjectFactory::createOperation(ProcessStore& store, const Value& info) {
  logger::trace("Process::createOperation({})", (info));
  auto f = store.getOperationFactory(info);
  if (!f) {
    return Value::error(logger::error("createOperation failed. Can not find appropreate operation factory."));
  }
  logger::info("Creating Operation({})", (info));
  return store.addOperation(f->create(info));
}


Value ObjectFactory::createContainer(ProcessStore& store, const Value& info) {
  logger::trace("Process::createContainer({})", (info));
  auto f = store.getContainerFactory(info);
  if (!f) {
    return Value::error(logger::error("createContainer failed. Can not find appropreate container factory."));
  }
  logger::info("Creating Container({})", (info));
  return store.addContainer(f->create(info));
}

Value ObjectFactory::createBroker(ProcessStore& store, const Value& ci) {
  logger::trace("Process::createBroker({})", (ci));

  auto f = store.getBrokerFactory(ci);
  if (!f) {
    return Value::error(logger::error("createBroker failed. Can not found appropreate broker factory."));
  }
  logger::info("Creating Broker({})", (ci));
  return store.addBroker(f->create(ci));
}

std::shared_ptr<BrokerAPI>  ObjectFactory::createBrokerProxy(ProcessStore& store, const Value& bi) {
  logger::trace("Process::createBrokerProxy({})", (bi));
  auto f = store.getBrokerFactory(bi);
  if (!f) {
    logger::error("createBrokerProxy failed. Can not found appropreate broker factory.");
    return nullptr;
  }
  logger::info("Creating BrokerProxy({})", (bi));
  return f->createProxy(bi);
}

Value ObjectFactory::createExecutionContext(ProcessStore& store, const Value& value) {
  auto f = store.getExecutionContextFactory(value);
  if (!f) {
    return Value::error(logger::error("createExecutionContext failed. Can not find appropreate execution context factory."));
  }
  logger::info("Creating Execution Context({})", (value));
  return store.addExecutionContext(f->create(value));
}