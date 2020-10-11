#include "nerikiri/fsm.h"
#include "nerikiri/logger.h"


using namespace nerikiri;

FSM::FSM(const Value& info) : Object(info) {
    if (info.at("states").isError()) {
        logger::warn("FSM::FSM(info) failed. FSM states are not specified.");
        _setNull();
    }
    if (!info.at("states").isListValue()) {
        logger::warn("FSM::FSM(info) failed. FSM states are not list style.");
        _setNull();
    }

    auto defaultState = info.at("defaultState");
    if (defaultState.isError() || (!defaultState.isStringValue())) {
        logger::warn("FSM::FSM(info) failed. FSM states are not specified.");
        _setNull();
    }

    if (!hasState(defaultState.stringValue())) {
        logger::warn("FSM::FSM(info) failed. FSM default state is not included in States.");
        _setNull();
    } else {
        currentStateName_ = defaultState.stringValue();
        info_["currentState"] = currentStateName_;
    }
}

Value FSM::info() const {
            
    auto v = Object::info();
    logger::trace("info---- {}", v);
    auto states = Value::list();
    v["states"].list_for_each([this,&states](auto stateValue) {
        stateValue["boundOperations"] = Value::list();
        for(auto opInfo : getBoundOperations(stateValue.at("name").stringValue())) {
            logger::trace("pushing------- {}", opInfo->getFullName());
            stateValue["boundOperations"].push_back(
                {
                    {"fullName", opInfo->getFullName()}
                }
            );
        }
         for(auto opPair : getBoundOperationWithArguments(stateValue.at("name").stringValue())) {
            logger::trace("pushing------- {}", opPair.first->getFullName());
            stateValue["boundOperations"].push_back(
                {
                    {"fullName", opPair.first->getFullName()},
                    {"argument", opPair.second}
                }
            );
        }

        stateValue["boundECStart"] = Value::list();
        stateValue["boundECStop"] = Value::list();
        for(auto ecPair : getBoundECs(stateValue.at("name").stringValue())) {
            //logger::trace("pushing------- {}", opInfo->getFullName());
            if (ecPair.first == "started") {
                stateValue["boundECStart"].push_back(
                    {
                        {"fullName", ecPair.second->getFullName()}
                    }
                );
            } else if (ecPair.first == "stopped") {
                stateValue["boundECStop"].push_back(
                    {
                    {"fullName", ecPair.second->getFullName()}
                    }
                );
            }
        }

        states.push_back(stateValue);
    });

    v["states"] = states;
    return v;
}

FSM::FSM() : Object() {}

FSM::~FSM() {}


bool FSM::hasState(const std::string& stateName) const {
    if (isNull()) {
        return false;
    }
    bool flag = false;
    info().at("states").const_list_for_each([&flag, &stateName](auto state) {
        if (getStringValue(state.at("name"), "") == stateName) flag = true;
    });
    return flag;
}

Value FSM::getFSMState() const {
    if (isNull()) {
        return Value::error("FSM::getFSMState() error. FSM is null.");
    }
    return currentStateName_;
}

Value FSM::setFSMState(const std::string& state) {
    logger::trace("FSM({})::setFSMState({})", getInstanceName(), state);
    if (isNull()) {
        return Value::error("FSM::setFSMState() error. FSM is null.");
    }
    if (!hasState(state)) {
        return Value::error("FSM::setFSMState() error. FSM does not have state " + state);
    }
    if (_isTransitable(currentStateName_, state)) {
        currentStateName_ = state;
        info_["currentState"] = state;
        _executeInState(currentStateName_);
    } else {
        logger::warn("FSM(" + getInstanceName() + ") tried to transit from " + currentStateName_ + " to " + state + " but can not transit.");
    }
    return currentStateName_;
}



Value FSM::_executeInState(const std::string& stateName) {
    std::vector<Value> retval;
    if (operations_.count(stateName) > 0) {
        for(auto& op : operations_.at(stateName)) {
            logger::trace(" - Executing Operation({})", op->getFullName());
            retval.emplace_back(op->execute());
        }
    }
    if (operationWithArguments_.count(stateName) > 0) {
        for(auto& opAndArgs : operationWithArguments_.at(stateName)) {
            auto op = opAndArgs.first;
            auto args = opAndArgs.second;
            logger::trace(" - Executing Operation({}) with Argument({})", op->getFullName(), args);
            args.object_for_each([op](auto key, auto& value) {
                op->putToArgument(key, value);
            });
            retval.emplace_back(op->execute());
        }
    }
    if (operationBrokers_.count(stateName) > 0) {
        for(auto& pair: operationBrokers_.at(stateName)) {
            retval.emplace_back(pair.second->executeAllOperation(pair.first));
        }
    }
    if (ecsStart_.count(stateName) > 0) {
        for(auto& ec : ecsStart_.at(stateName)) {
            logger::trace(" - StartingEC({})", ec->getFullName());
            retval.emplace_back(ec->start());
        }
    }
    if (ecStartBrokers_.count(stateName) > 0) {
        for(auto& pair : ecStartBrokers_.at(stateName)) {
            retval.emplace_back( pair.second->setExecutionContextState(pair.first, "started") );
        }
    }
    if (ecsStop_.count(stateName) > 0) {
        for(auto& ec : ecsStop_.at(stateName)) {
            logger::trace(" - StoppingEC({})", ec->getFullName());
            retval.emplace_back(ec->stop());
        }
    }
    if (ecStopBrokers_.count(stateName) > 0) {
        for(auto& pair : ecStopBrokers_.at(stateName)) {
            retval.emplace_back( pair.second->setExecutionContextState(pair.first, "stopped") );
        }
    }
    return retval;
}


bool FSM::hasInputConnectionRoute(const Value& conInfo) const {
    for(auto it = connections_.begin();it != connections_.end();++it) {
        if (it->info().at("output").at("info").at("fullName") == conInfo.at("output").at("info").at("fullName")) {
            return true;
        }
    }
    return false;
}

bool FSM::hasInputConnectionName(const Value& conInfo) const {
    for(auto it = connections_.begin();it != connections_.end();++it) {
        if (it->info().at("name") == conInfo.at("name")) {
            return true;
        }
    }
    return false;
}

bool FSM::_isTransitable(const std::string& current, const std::string& next) {
    if (isNull()) {
        return false;
    }
    bool flag = false;
    info().at("states").const_list_for_each([&flag, &current, &next](auto& value) {
        if (getStringValue(value.at("name"), "") == current) {
            auto transit = value.at("transit");
            transit.list_for_each([&flag, &next](auto& tv) {
                if (tv.isStringValue() && tv.stringValue() == next) {
                    flag = true;
                }
            });
        }
    });
    return flag;
}

Value FSM::getFSMStates() const {
    if (isNull()) {
        return Value::error("FSM::getFSMStates() error. FSM is null.");
    }
    return info().at("states");
}

Value FSM::bindStateToOperation(const std::string& stateName, const std::shared_ptr<OperationBase>& op) {
    if (isNull()) {
        return Value::error(logger::error("FSM::bindStateToOperation(" + stateName + ") error. FSM is null."));
    }
    if (op->isNull()) {
        return Value::error(logger::error("FSM::bindStateToOperation(" + stateName + ") error. Operation is null."));
    }
    this->operations_[stateName].push_back(op);
    return op->info();
}

Value FSM::bindStateToOperation(const std::string& stateName, const std::shared_ptr<OperationBase>& op, const Value& argumentInfo) {
    if (isNull()) {
        return Value::error(logger::error("FSM::bindStateToOperation(" + stateName + ") error. FSM is null."));
    }
    if (op->isNull()) {
        return Value::error(logger::error("FSM::bindStateToOperation(" + stateName + ") error. Operation is null."));
    }
    this->operationWithArguments_[stateName].push_back({op, argumentInfo});
    return op->info();
}


std::vector<std::shared_ptr<OperationBase>> FSM::getBoundOperations(const std::string& stateName) const {
    if (operations_.count(stateName) > 0) {
        return this->operations_.at(stateName);
    }
    return {};
}

std::vector<std::pair<std::shared_ptr<OperationBase>, Value>> FSM::getBoundOperationWithArguments(const std::string& stateName) const {
    if (operationWithArguments_.count(stateName) > 0) {
        return this->operationWithArguments_.at(stateName);
    }
    return {};
}



std::vector<std::pair<std::string, std::shared_ptr<ExecutionContext>>> FSM::getBoundECs(const std::string& stateName) const {

    std::vector<std::pair<std::string, std::shared_ptr<ExecutionContext>>>  v;
    if (ecsStart_.count(stateName) > 0) {
        for(auto ec: ecsStart_.at(stateName)) {
            v.push_back({std::string("started"), ec});
        }
    }
    if (ecsStop_.count(stateName) > 0) {
        for(auto ec: ecsStop_.at(stateName)) {
            v.push_back({std::string("stopped"), ec});
        }
    }
    return v;
}


Value FSM::bindStateToECStart(const std::string& stateName, const std::shared_ptr<ExecutionContext>& ec) {
    if (isNull()) {
        return Value::error(logger::error("FSM::bindStateToEC(" + stateName + ") error. FSM is null."));
    }
    if (ec->isNull()) {
        return Value::error(logger::error("FSM::bindStateToEC(" + stateName + ") error. EC is null."));
    }
    this->ecsStart_[stateName].push_back(ec);
    return ec->info();
    //return Value::error(logger::error("FSM::bindStateToOperation(" + stateName + ") error. Not Implemented."));
}

Value FSM::bindStateToECStop(const std::string& stateName, const std::shared_ptr<ExecutionContext>& ec) {
    if (isNull()) {
        return Value::error(logger::error("FSM::bindStateToEC(" + stateName + ") error. FSM is null."));
    }
    if (ec->isNull()) {
        return Value::error(logger::error("FSM::bindStateToEC(" + stateName + ") error. EC is null."));
    }
    this->ecsStop_[stateName].push_back(ec);
    return ec->info();
    //return Value::error(logger::error("FSM::bindStateToOperation(" + stateName + ") error. Not Implemented."));
}