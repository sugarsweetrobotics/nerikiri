
#include <sstream>
#include "nerikiri/nerikiri.h"
#include "nerikiri/functional.h"
#include "nerikiri/process.h"
#include "nerikiri/logger.h"
#include "nerikiri/signal.h"
#include "nerikiri/naming.h"

#include <iostream>

using namespace nerikiri;
using namespace nerikiri::logger;




Value ProcessStore::info() const { return process_->info(); }

Value ProcessStore::getContainerInfos() {
  return {nerikiri::map<Value, std::shared_ptr<ContainerBase>>(containers_, [](auto& ctn) { return ctn->info(); })};
}


Value ProcessStore::addContainer(std::shared_ptr<ContainerBase> container) {
  trace("Process::addContainer()");
  auto name = nerikiri::numbering_policy<std::shared_ptr<ContainerBase>>(containers_, container->info().at("name").stringValue(), ".ctn");
  if (container->isNull()) {
    return Value::error(logger::error("Container is null."));
  }
  container->setInstanceName(name);
  containers_.push_back(container);
  return container->info();
}

ContainerBase& ProcessStore::getContainer(const Value& info) {
  return nerikiri::find_first<ContainerBase&, std::shared_ptr<ContainerBase>>(containers_,
    [&info](auto& c){return c->info().at("instanceName") == info.at("instanceName");},
    [] (auto& c) -> ContainerBase& { return *(c.get()); },
    ContainerBase::null
    );
}

Value ProcessStore::addOperation(std::shared_ptr<Operation> op) {
  logger::trace("Process::addOperation({})", str(op->info()));
  auto name = nerikiri::numbering_policy<std::shared_ptr<Operation>>(operations_, op->info().at("name").stringValue(), ".ope");
  op->setInstanceName(name);
  if (!getOperation(op->info()).isNull()) {
    return Value::error(logger::error("Process::addOperation({}) Error. Process already has the same name operation", op->info().at("name").stringValue()));
  }
  operations_.push_back(op);
  return op->info();
}

OperationBaseBase& ProcessStore::getOperation(const OperationInfo& oi) {
  if (oi.isError()) return Operation::null;
  Operation& null = Operation::null;
  auto& name = oi.at("instanceName");
  auto pos = name.stringValue().find(":");
  if (pos != std::string::npos) {
    auto containerName = name.stringValue().substr(0, pos);
    auto operationName = name.stringValue().substr(pos+1);
    std::cout << "Con:" << containerName << ", " << operationName << std::endl;
    return getContainer({{"instanceName", containerName}}).getOperation({{"instanceName", operationName}});
  } else {
    for(auto& op : operations_) {
      if (op->info().at("instanceName") == name) return *op;
    }
  }
  return Operation::null;
}

std::shared_ptr<OperationFactory> ProcessStore::getOperationFactory(const Value& oi) {
  for(auto& opf : operationFactories_) {
    if (opf->typeName() == oi.at("name").stringValue()) {
      return opf;
    }
  }
  return nullptr;
}

Value ProcessStore::getOperationInfos() {
  auto ops = nerikiri::map<Value, std::shared_ptr<Operation>>(operations_, [](auto& op) { return op->info();});
  for(auto& c : this->containers_) {
    auto infos = c->getOperationInfos();
    ops.insert(ops.end(), infos.begin(), infos.end());
  }
  return ops;
}


Value ProcessStore::getConnectionInfos() const {
  return nerikiri::map<std::pair<std::string, Value>, std::shared_ptr<Operation>>(operations_, [](auto& op) -> std::pair<std::string, Value> {
    return {op->info().at("name").stringValue(), op->getConnectionInfos()};
  });
}


Value ProcessStore::addExecutionContext(std::shared_ptr<ExecutionContext> ec) {
  auto name = nerikiri::numbering_policy<std::shared_ptr<ExecutionContext>>(executionContexts_, ec->info().at("name").stringValue(), ".ec");
  ec->setInstanceName(name);
  executionContexts_.push_back(ec);
  return ec->info();
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
      return {{"name", cf->typeName()}};
    }
  }
  return Value::error(logger::error("ProcessStore::addContainerOperationFactory({}) can not find appropreate Container Factory.", cof->typeName()));
}


std::shared_ptr<ContainerFactoryBase> ProcessStore::getContainerFactory(const Value& info) {
  for(auto f : containerFactories_) {
    if (f->typeName() == info.at("name").stringValue()) {
      return f;
    }
  }
  logger::error("getContainerFactory failed. Can not find appropreate container factory.");
  return nullptr;
}

std::shared_ptr<ExecutionContextFactory> ProcessStore::getExecutionContextFactory(const Value& info) {
  for(auto f : executionContextFactories_) {
    if (f->typeName() == info.at("name").stringValue()) {
      return f;
    }
  }
  logger::error("getExecutionContext failed. Can not find appropreate execution context factory.");
  return nullptr;
}
