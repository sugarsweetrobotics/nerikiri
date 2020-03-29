#include "nerikiri/container.h"

#include "nerikiri/functional.h"

using namespace nerikiri;

ContainerBase& ContainerBase::addOperation(std::shared_ptr<ContainerOperationBase> operation) { 
    if (!getOperation(operation->getContainerOperationInfo()).isNullContainerOperation()) {
        logger::error("ContainerBase::addOperation({}) failed. ContainerOperation with same name is registered.", str(operation->getContainerOperationInfo()));
    } else {
        operation->setContainer(this);
        operations_.push_back(operation); 
    }
    return *this;
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

ContainerBase ContainerBase::null("null", {{"name", "null"}});