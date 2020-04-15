#include "nerikiri/container.h"
#include "nerikiri/containerfactory.h"
#include "nerikiri/containeroperation.h"
#include "nerikiri/containeroperationfactory.h"
#include "nerikiri/functional.h"
#include "nerikiri/naming.h"

using namespace nerikiri;

Value ContainerBase::addOperation(std::shared_ptr<ContainerOperationBase> operation) { 
    auto name = nerikiri::numbering_policy<std::shared_ptr<ContainerOperationBase>>(operations_, operation->info().at("name").stringValue(), ".ope");
    operation->setInstanceName(name);
    if (!getOperation(operation->getContainerOperationInfo())->isNull()) {
        return Value::error(logger::error("ContainerBase::addOperation({}) failed. ContainerOperation with same name is registered.", str(operation->getContainerOperationInfo())));
    }
    operation->setContainer(this);
    operations_.push_back(operation); 
    return operation->info();;
}

std::vector<Value> ContainerBase::getOperationInfos() const {
    return nerikiri::map<Value, ContainerOperationBase_ptr>(operations_, [](auto op) { return op->getContainerOperationInfo();});
}

 std::shared_ptr<OperationBase> ContainerBase::getOperation(const Value& info) const {
    for(auto op: operations_) {
        if (op->getContainerOperationInfo().at("instanceName") == info.at("instanceName")) {
            return op;
        }
    }
    return OperationBase::null;
}


std::shared_ptr<ContainerBase> ContainerBase::null = std::make_shared<ContainerBase>();

std::shared_ptr<ContainerOperationFactoryBase> ContainerBase::getContainerOperationFactory(const Value& info) {
    if (this->isNull()) {
        logger::error("ContainerBase::getContainerOperationFactory failed. Container is null.");
        return nullptr;
    }
    for(auto& opf : parentFactory_->operationFactories_) {
        if (opf->typeName() == opf->containerTypeName() + ":"+ info.at("name").stringValue()) {
        return opf;
        }
    }
    return nullptr;
}

Value ContainerBase::createContainerOperation(const Value& info) {
    logger::trace("ContainerBase::createContainerOperation({})", str(info));
    auto f = getContainerOperationFactory(info);
    if (!f) {
        return Value::error(logger::error("createContainerOperation failed. Can not find appropreate operation factory."));
    }
    logger::info("Creating ContainerOperation({})", str(info));
    return addOperation(f->create());
}
