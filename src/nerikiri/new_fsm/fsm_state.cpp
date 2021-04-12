

#include "fsm_state.h"

using namespace nerikiri;


FSMState::FSMState(FSMAPI* owner, const std::string& name, const std::shared_ptr<OperationAPI>& operation, const std::vector<std::string>& transitableNames, const bool is_active) 
: FSMStateAPI("FSMState", "FSMState", name), owner_(owner), operation_(operation), transitableStateNames_(transitableNames)
{}

FSMState::~FSMState() {
    finalize();
}

void FSMState::finalize() {

}

Value FSMState::info() const {
    auto v = Object::info();
    /*
    v["boundOperations"] = nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(operations_, [](auto op) {
        return op->info();
    });
    v["boundECStates"] = nerikiri::functional::map<Value, std::shared_ptr<ECStateAPI>>(ecStates_, [](auto op) {
        return op->info();
    });
    */
    return v;
}

bool FSMState::isActive() const { /* return owner_->isActiveState(fullName()); */ }

bool FSMState::activate() {
    operation_->inlet("stateName")->put(fullName());
    return !operation_->execute().isError();
}

bool FSMState::deactivate() {
    // is_active_ = false;
    return true;
}

bool FSMState::isTransitable(const std::string& stateName) const {
    return std::find(transitableStateNames_.begin(), transitableStateNames_.end(), stateName) != transitableStateNames_.end();
}

Value FSMState::bind(const std::shared_ptr<OperationAPI>& op) {
    /*
    if (op->isNull()) return Value::error(logger::error("FSMState({})::bind(OperationAPI) failed. Operation is null.", fullName()));
    return operations_.emplace_back(op) -> info();
    */
   
}

Value FSMState::bind(const std::shared_ptr<OperationAPI>& op, const Value& arg) {
    /*
    if (op->isNull()) return Value::error(logger::error("FSMState({})::bind(OperationAPI) failed. Operation is null.", fullName()));
    //operationAndArgs_.push_back({op, arg});
    return operations_.emplace_back(operationWithArgument(op, arg))->info();
    */
}

Value FSMState::bind(const std::shared_ptr<ECStateAPI>& ecs) {
    /*
    if (ecs->isNull()) return Value::error(logger::error("FSMState({})::bind(ECStateAPI) failed. ECState is null.", fullName()));
    return ecStates_.emplace_back(ecs) -> info();
    */
}

Value FSMState::unbind(const std::shared_ptr<OperationAPI>& op) {
    /*
    if (op->isNull()) return Value::error(logger::error("FSMState({})::unbind(OperationAPI) failed. Operation is null.", fullName()));
    operations_.erase(std::remove(operations_.begin(), operations_.end(), op), operations_.end());
    return op->info();
    */
}

Value FSMState::unbind(const std::shared_ptr<ECStateAPI>& e)  {
    /*
    if (e->isNull()) return Value::error(logger::error("FSMState({})::unbind(ECStateAPI) failed. ECState is null.", fullName()));
    ecStates_.erase(std::remove(ecStates_.begin(), ecStates_.end(), e), ecStates_.end());
    return e->info();
    */
}

std::shared_ptr<OperationInletAPI> FSMState::inlet() { return operation_->inlet("stateName");}

std::shared_ptr<OperationOutletAPI> FSMState::outlet() { return operation_->outlet();}

std::vector<std::shared_ptr<OperationAPI>> FSMState::boundOperations() { /* return operations_; */ }

std::vector<std::shared_ptr<ECStateAPI>> FSMState::boundECStates() { /* return ecStates_; */ }
