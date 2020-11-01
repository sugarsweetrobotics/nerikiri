#include <nerikiri/functional.h>
#include <nerikiri/fsm.h>
#include <nerikiri/logger.h>


using namespace nerikiri;

FSM::FSM(const Value& info) : FSMAPI(info) {
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

    setFsmStateActive(currentState);

    if (currentState->isNull()) {
        logger::warn("FSM::FSM(info) failed. FSM default state is not included in the given states.");
        _setNull();
        return;
    } 
    //info_["currentState"] = currentStateName_;
}

bool FSM::_setupStates(const Value& info) {

    if (info.at("states").isError()) {
        logger::warn("FSM::FSM(info) failed. FSM states are not specified.");
        return false;
    }
    if (!info.at("states").isListValue()) {
        logger::warn("FSM::FSM(info) failed. FSM states must be list style.");
        return false;
    }
    this->states_ = info.at("states").const_list_map<std::shared_ptr<FSMStateAPI>>([this](auto stateInfo) -> std::shared_ptr<FSMStateAPI> {
        if (!stateInfo.hasKey("name")) {
            logger::error("FSM::FSM(info) failed. Each FSM state info must have 'name' string type member to specify the name of the state");
            return std::make_shared<NullFSMState>();
        }
        if (!stateInfo.hasKey("transit") || stateInfo.at("transit").isListValue()) {
            logger::error("FSM::FSM(info) failed. Each FSM state info must have 'transit' list type member to specify the transitions of the state");
            return std::make_shared<NullFSMState>();
        }
        return std::make_shared<FSMState>(stateInfo.at("name").stringValue(), stateInfo.at("transit").template const_list_map<std::string>([](auto t) {
            return t.stringValue();
        }));
    });
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
    currentFsmState()->deactivate();
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
