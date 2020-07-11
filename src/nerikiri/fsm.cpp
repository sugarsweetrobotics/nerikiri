#include "nerikiri/fsm.h"



using namespace nerikiri;

FSM::FSM(const Value& info) : Object(info) {}


FSM::FSM() : Object() {}

FSM::~FSM() {}

Value FSM::getFSMState() const {
    if (isNull()) {
        return Value::error("FSM::getFSMState() error. FSM is null.");
    }
    return Value::error("FSM::getFSMState() is not implemented");
}

Value FSM::setFSMState(const std::string& state) {
    if (isNull()) {
        return Value::error("FSM::setFSMState() error. FSM is null.");
    }
    return Value::error("FSM::setFSMState() is not implemented");
}