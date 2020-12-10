#include <nerikiri/functional.h>
#include <nerikiri/fsm.h>
#include <nerikiri/logger.h>
#include "fsm_state.h"

using namespace nerikiri;

class FSMFactory : public FSMFactoryAPI {
public:
    FSMFactory(const std::string& fullName) : FSMFactoryAPI("FSMFactory", "GenericFSM", fullName) {} // For Null
public:
    virtual ~FSMFactory() {}

public:
    virtual std::shared_ptr<FSMAPI> create(const std::string& _fullName, const Value& extInfo) const override {
        return fsm(_fullName, extInfo);
    }
};

/**
 * 
 */ 
class NK_API FSM : public FSMAPI{
private:
    /*
    std::vector<Connection> connections_;
    std::string currentStateName_;
    
    std::map<std::string, std::vector<std::shared_ptr<OperationBase>>> operations_;
    std::map<std::string, std::vector<std::pair<std::shared_ptr<OperationBase>, Value>>> operationWithArguments_;

    std::map<std::string, std::vector<std::pair<std::string, std::shared_ptr<BrokerAPI>>>> operationBrokers_;
    std::map<std::string, std::vector<std::shared_ptr<ExecutionContextAPI>>> ecsStart_;
    std::map<std::string, std::vector<std::pair<std::string, std::shared_ptr<BrokerAPI>>>> ecStartBrokers_;
    std::map<std::string, std::vector<std::shared_ptr<ExecutionContextAPI>>> ecsStop_;
    std::map<std::string, std::vector<std::pair<std::string, std::shared_ptr<BrokerAPI>>>> ecStopBrokers_;
    */
    std::vector<std::shared_ptr<FSMStateAPI>> states_;
    
    bool _setupStates(const Value& info);
public:
    virtual std::vector<std::shared_ptr<FSMStateAPI>> fsmStates() const override { 
        return states_;
    }

    virtual std::shared_ptr<FSMStateAPI> fsmState(const std::string& stateName) const override {
        auto s = nerikiri::functional::find<std::shared_ptr<FSMStateAPI>>(states_, [&stateName](auto s) {
            return s->fullName() == stateName;
        });
        if (s) return s.value();
        logger::error("FSM({})::fsmState({}) failed. State not found.", fullName(), stateName);
        return nullFSMState();
    }

    virtual std::shared_ptr<FSMStateAPI> currentFsmState() const override {
        auto s = nerikiri::functional::find<std::shared_ptr<FSMStateAPI>>(states_, [](auto s) {
            return s->isActive();
        });
        if (s) return s.value();
        return nullFSMState();
    }

    Value addFSMState(const std::string& name) {
        if (!fsmState(name)->isNull()) {
            return Value::error(logger::error("FSM::addFSMState({}) failed. Same name state already exists"));
        } 
        auto s = std::make_shared<FSMState>(name);
        states_.push_back(s);
        return s->info();
    }

    Value deleteFSMState(const std::string& name) {
        auto s = fsmState(name);
        if (!s->isNull()) {
            return Value::error(logger::error("FSM::addFSMState({}) failed. Same name state already exists"));
        } 
        states_.erase(std::remove(states_.begin(), states_.end(), s), states_.end());
        return s->info();
    }

public:
    FSM(const std::string& fullName, const Value& info);

    FSM();

    virtual Value info() const override ;

    virtual ~FSM();

private:
    bool _isTransitable(const std::string& current, const std::string& next);

public:

    Value getFSMState() const;
    
    virtual Value setFSMState(const std::string& stateName) override;

    Value getFSMStates() const;

    bool hasState(const std::string& stateName) const;

    /*
    Value bindStateToOperation(const std::string& stateName, const std::shared_ptr<OperationBase>& op);

    Value bindStateToOperation(const std::string& stateName, const std::shared_ptr<OperationBase>& op, const Value& argumentInfo);
    */
    /*
    std::vector<std::shared_ptr<OperationBase>> getBoundOperations(const std::string& stateName) const;

    std::vector<std::pair<std::shared_ptr<OperationBase>, Value>> getBoundOperationWithArguments(const std::string& stateName) const;

    std::vector<std::pair<std::string, std::shared_ptr<ExecutionContext>>> getBoundECs(const std::string& stateName) const;
    
    Value bindStateToECStart(const std::string& stateName, const std::shared_ptr<ExecutionContext>& ec);
    
    Value bindStateToECStop(const std::string& stateName, const std::shared_ptr<ExecutionContext>& ec);

    bool hasInputConnectionRoute(const Value& conInfo) const;
    bool hasInputConnectionName(const Value& conInfo) const;
    

    Value addInputConnection(Connection&& con) {
        connections_.emplace_back(std::move(con));
        return con.info();
    }

    Value removeInputConnection(const Value& connectionInfo) {
        for(auto it = connections_.begin(); it != connections_.end();) {
            if (it->info().at("name") == connectionInfo.at("name")) {
                it = connections_.erase(it);
                return connectionInfo;
            } else {
                ++it;
            }
        }
        return connectionInfo;
    }
    */
private:
    //Value _executeInState(const std::string& stateName);
};



FSM::FSM(const std::string& fullName, const Value& info) : FSMAPI("GenericFSM", "GenericFSM", fullName) {
    logger::trace("FSM::{} called", __func__);
    if (!_setupStates(info) ) {
        _setNull();
        return;
    }

    if (!info.hasKey("defaultState")) {
        logger::warn("FSM::FSM(info) failed. FSM default state is not specified.");
        _setNull();
        return;
    }
    auto currentState = fsmState(info.at("defaultState").stringValue());

    //currentState->activate();
    // setFsmStateActive(currentState);

    if (currentState->isNull()) {
        logger::warn("FSM::FSM(info) failed. FSM default state is not included in the given states.");
        _setNull();
        return;
    } 
    if (!currentState->isActive()) {
        logger::warn("FSM::FSM(info) failed. FSM default state is not included in the given states.");
        _setNull();
        return;
    } 
    //info_["currentState"] = currentStateName_;
}

bool FSM::_setupStates(const Value& info) {
    logger::trace("FSM::{}(info={}) called", __func__, info);

    if (info.at("defaultState").isError()) {
        logger::warn("FSM::FSM(info) failed. FSM default state is not specified.");
        logger::trace("FSM::{} exit", __func__);
        return false;
    }
    auto defaultStateName = Value::string(info.at("defaultState"));

    if (info.at("states").isError()) {
        logger::warn("FSM::FSM(info) failed. FSM states are not specified.");
        logger::trace("FSM::{} exit", __func__);
        return false;
    }
    if (!info.at("states").isListValue()) {
        logger::warn("FSM::FSM(info) failed. FSM states must be list style.");
        logger::trace("FSM::{} exit", __func__);
        return false;
    }
    this->states_ = info.at("states").const_list_map<std::shared_ptr<FSMStateAPI>>([this, defaultStateName](auto stateInfo) -> std::shared_ptr<FSMStateAPI> {
        if (!stateInfo.hasKey("name")) {
            logger::error("FSM creating FSMState in FSM::_setupStates(info) failed. Each FSM state info must have 'name' string type member to specify the name of the state");
            return nullFSMState();
        }
        if (!stateInfo.hasKey("transit") || !stateInfo.at("transit").isListValue()) {
            logger::error("FSM creating FSMState in FSM::_setupStates(info) failed. Each FSM state info must have 'transit' LIST type member to specify the transitions of the state");
            return nullFSMState();
        }
        logger::trace("FSM({}) create FSMState({}, {})", fullName(), stateInfo.at("name"), stateInfo.at("transit"));
        auto state = std::make_shared<FSMState>(stateInfo.at("name").stringValue(), stateInfo.at("transit").template const_list_map<std::string>([this](auto t) {
            return t.stringValue();
        }), stateInfo.at("name").stringValue() == defaultStateName);
        state->setOwner(this);
        return state;
    });
    logger::trace("FSM::{} exit", __func__);
    return true;
}

Value FSM::info() const {
    auto v = Object::info();
    v["states"] = nerikiri::functional::map<Value, std::shared_ptr<FSMStateAPI>>(states_, [](auto state) {
        return state->info();
    });
    return v;
    /*
    v["states"].list_for_each([this, &states](auto stateValue) {
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
    */

    //v["states"] = states;
    //return v;
}

FSM::FSM() : FSMAPI() {}

FSM::~FSM() {}


bool FSM::hasState(const std::string& stateName) const {
    return static_cast<bool>(nerikiri::functional::find<std::shared_ptr<FSMStateAPI>>(states_, [&stateName](auto s) { return s->fullName() == stateName; }));
}


Value FSM::setFSMState(const std::string& stateName) {
    logger::trace("FSM({})::setFSMState({})", fullName(), stateName);
    if (!currentFsmState()->isTransitable(stateName)) {
        return Value::error(logger::warn("FSM({}) can not transit from {} to {}", fullName(), currentFsmState()->fullName(), stateName));
    }
    auto tgtState = fsmState(stateName);
    if (tgtState->isNull()) {
        return Value::error(logger::error("FSM::setFSMState() error. FSM does not have state {}", stateName));
    }
    //currentFsmState()->deactivate();
    tgtState->activate();
    return tgtState->info();
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



std::shared_ptr<FSMAPI> nerikiri::fsm(const std::string& fullname, const Value& info) {
    return std::make_shared<FSM>(fullname, info);
}



std::shared_ptr<FSMFactoryAPI> nerikiri::fsmFactory(const std::string& fullName) {
    return std::make_shared<FSMFactory>(fullName);
}
