#include "nerikiri/container.h"

#include "nerikiri/functional.h"
#include "nerikiri/naming.h"

using namespace nerikiri;

Value ContainerBase::addOperation(std::shared_ptr<ContainerOperationBase> operation) { 
    auto name = nerikiri::numbering_policy<std::shared_ptr<ContainerOperationBase>>(operations_, operation->info().at("name").stringValue(), ".ope");
    operation->setInstanceName(name);
    if (!getOperation(operation->getContainerOperationInfo()).isNullContainerOperation()) {
        return Value::error(logger::error("ContainerBase::addOperation({}) failed. ContainerOperation with same name is registered.", str(operation->getContainerOperationInfo())));
    }
    operation->setContainer(this);
    operations_.push_back(operation); 
    return operation->info();;
}

std::vector<Value> ContainerBase::getOperationInfos() const {
    return nerikiri::map<Value, ContainerOperationBase_ptr>(operations_, [](auto op) { return op->getContainerOperationInfo();});
}

ContainerOperationBase& ContainerBase::getOperation(const Value& info) const {
    for(auto op: operations_) {
        if (op->getContainerOperationInfo().at("operationName") == info.at("name")) {
            return *op;
        }
    }
    return *ContainerOperationBase::null;
}

ContainerOperationBase* ContainerOperationBase::null = new ContainerOperation<int>();

ContainerBase ContainerBase::null(nullptr, "null", {{"name", "null"}});

std::shared_ptr<ContainerOperationFactoryBase> ContainerBase::getContainerOperationFactory(const Value& info) {
    for(auto& opf : parentFactory_->operationFactories_) {
        if (opf->typeName() == info.at("name").stringValue()) {
        return opf;
        }
    }
    return nullptr;
}

Value ContainerBase::createContainerOperation(const Value& info) {
    logger::trace("Process::createOperation({})", str(info));
    auto f = getContainerOperationFactory(info);
    if (!f) {
        return Value::error(logger::error("createOperation failed. Can not find appropreate operation factory."));
    }
    logger::info("Creating Operation({})", str(info));
    return addOperation(f->create());
}
