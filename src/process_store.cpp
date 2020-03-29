
#include <sstream>
#include "nerikiri/nerikiri.h"
#include "nerikiri/functional.h"
#include "nerikiri/process.h"
#include "nerikiri/logger.h"
#include "nerikiri/signal.h"


#include <iostream>

using namespace nerikiri;
using namespace nerikiri::logger;




Value ProcessStore::info() const { return process_->info(); }

Value ProcessStore::getContainerInfos() {
  return {nerikiri::map<Value, std::shared_ptr<ContainerBase>>(containers_, [](auto& ctn) { return ctn->info(); })};
}


Value ProcessStore::addContainer(std::shared_ptr<ContainerBase> container) {
  trace("Process::addContainer()");
  if (container->isNull()) {
    return Value::error(logger::error("Container is null."));
  }
  containers_.emplace_back(container);
  return container->info();
}


ContainerBase& ProcessStore::getContainerByName(const std::string& name) {
  auto cs = nerikiri::filter<std::shared_ptr<ContainerBase>>(containers_, [&name](auto& c){return c->info().at("name").stringValue() == name;});
  if (cs.size() == 1) return *cs[0];
  if (cs.size() == 0) {
    logger::warn("Process::getContainerByName({}) failed. Not found.", name);
    return ContainerBase::null;
  } else {
    logger::warn("Process::getContainerByName({}) failed. Multiple containers with the same name are found.", name);
    return ContainerBase::null;

  }
}


Value ProcessStore::addOperation(std::shared_ptr<Operation> op) {
  logger::trace("Process::addOperation({})", str(op->info()));
  if (!getOperation(op->info()).isNull()) {
    return Value::error(logger::error("Process::addOperation({}) Error. Process already has the same name operation", op->info().at("name").stringValue()));
  }
  operations_.push_back(op);
  return op->info();
}

OperationBaseBase& ProcessStore::getOperation(const OperationInfo& oi) {
  Operation& null = Operation::null;
  auto& name = oi.at("name");
  auto pos = name.stringValue().find(":");
  if (pos != std::string::npos) {
    auto containerName = name.stringValue().substr(0, pos);
    auto operationName = name.stringValue().substr(pos+1);
    std::cout << "Con:" << containerName << ", " << operationName << std::endl;
    return getContainerByName(containerName).getOperation({{"name", operationName}});
  } else {
    for(auto& op : operations_) {
      if (op->info().at("name") == name) return *op;
    }
  }
  return Operation::null;
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

}


