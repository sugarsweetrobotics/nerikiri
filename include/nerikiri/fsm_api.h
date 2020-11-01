#pragma once

#include <nerikiri/fsm_state_api.h>

namespace nerikiri {

    /**
     * 
     */
    class NK_API FSMAPI : public Object {
    public:
        FSMAPI(): Object() {}

        FSMAPI(const std::string& typeName, const std::string& fullName): Object(typeName, fullName) {}

        FSMAPI(const Value& info): Object(info) {}

        virtual ~FSMAPI() {}

    public:
        virtual std::shared_ptr<FSMStateAPI> currentFsmState() const = 0;

        virtual std::shared_ptr<FSMStateAPI> fsmState(const std::string& stateName) const = 0;

        virtual std::vector<std::shared_ptr<FSMStateAPI>> fsmStates() const = 0;
        
        virtual Value setFSMState(const std::string& stateName) = 0;

    protected:
        virtual bool setFsmStateActive(const std::shared_ptr<FSMStateAPI>& state) {
            return state->_setActive();
        }

    };

    class NK_API NullFSM : public FSMAPI {
    public:
        NullFSM(): FSMAPI() {}

        virtual ~NullFSM() {}

    public:
        virtual std::shared_ptr<FSMStateAPI> currentFsmState() const override {
            logger::error("NullFSM::{}() failed. FSM is null.", __FUNCTION__);
            return std::make_shared<NullFSMState>();
        }

        std::vector<std::shared_ptr<FSMStateAPI>> fsmStates() const override {
            logger::error("NullFSM::{}() failed. FSM is null.", __FUNCTION__);
            return {};
        }
        
        virtual std::shared_ptr<FSMStateAPI> fsmState(const std::string& stateName) const override {
            logger::error("NullFSM::{}() failed. FSM is null.", __FUNCTION__);
            return std::make_shared<NullFSMState>();
        }

        virtual Value setFSMState(const std::string& stateName) override {
           return Value::error(logger::debug("NullFSM::setFSMState({}) called.", stateName));
        }


    };
    

}