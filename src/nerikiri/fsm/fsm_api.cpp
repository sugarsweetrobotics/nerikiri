#include <nerikiri/fsm_api.h>

using namespace nerikiri;


class NK_API NullFSM : public FSMAPI {
public:
    NullFSM(): FSMAPI() {}

    virtual ~NullFSM() {}

public:
    virtual std::shared_ptr<FSMStateAPI> currentFsmState() const override {
        logger::error("NullFSM::{}() failed. FSM is null.", __FUNCTION__);
        return nullFSMState();
    }

    std::vector<std::shared_ptr<FSMStateAPI>> fsmStates() const override {
        logger::error("NullFSM::{}() failed. FSM is null.", __FUNCTION__);
        return {};
    }
    
    virtual std::shared_ptr<FSMStateAPI> fsmState(const std::string& stateName) const override {
        logger::error("NullFSM::{}() failed. FSM is null.", __FUNCTION__);
        return nullFSMState();
    }

    virtual Value setFSMState(const std::string& stateName) override {
        return Value::error(logger::debug("NullFSM::setFSMState({}) called.", stateName));
    }
        


};

std::shared_ptr<FSMAPI> nerikiri::nullFSM() { 
    logger::error("nullFSM() called. Created FSM is null.");
    return std::make_shared<NullFSM>();
}