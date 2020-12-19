#include <nerikiri/fsm_state_api.h>
#include <nerikiri/ec_state_api.h>
#include <nerikiri/operation_api.h>

using namespace nerikiri;



class NK_API NullFSMState : public FSMStateAPI {
public:
    NullFSMState() {}
    virtual ~NullFSMState() {}

public:

    virtual bool isActive() const override {
        logger::error("NullFSMState()::isActive() failed. FSMState is null.");
        return false;
    }

    virtual bool activate() override {
        logger::error("NullFSMState()::activate() failed. FSMState is null.");
        return false;
    }

    virtual bool deactivate() override {
        logger::error("NullFSMState()::{}() failed. FSMState is null.", __FUNCTION__);
        return false;
    }


    virtual bool isTransitable(const std::string& stateName) const override {
        logger::error("NullFSMState()::isTransitable(string stateName({})) failed. FSMState is null.", stateName);
        return false;
    }

    virtual Value bind(const std::shared_ptr<OperationAPI>& op) override {
        return Value::error(logger::error("NullFSMState()::bind(OperationAPI op({})) failed. FSMState is null.", op->fullName()));
    }

    virtual Value bind(const std::shared_ptr<OperationAPI>& op, const Value& arg) override {
        return Value::error(logger::error("NullFSMState()::bind(OperationAPI op({})) failed. FSMState is null.", op->fullName()));
    }

    virtual Value bind(const std::shared_ptr<ECStateAPI>& ecs) override {
        return Value::error(logger::error("NullFSMState()::bind(ECStateAPI op({})) failed. FSMState is null.", ecs->fullName()));
    }

    virtual Value unbind(const std::shared_ptr<OperationAPI>& op) override {
        return Value::error(logger::error("NullFSMState()::unbind(OperationAPI op({})) failed. FSMState is null.", op->fullName()));
    }

    virtual Value unbind(const std::shared_ptr<ECStateAPI>& ecs) override {
        return Value::error(logger::error("NullFSMState()::unbind(ECStateAPI op({})) failed. FSMState is null.", ecs->fullName()));           
    }

    virtual std::vector<std::shared_ptr<OperationAPI>> boundOperations() override {
        logger::error("NullFSMState({})::boundOperations() failed. FSMState is null.", fullName());
        return {};
    }
    
    virtual std::vector<std::shared_ptr<ECStateAPI>> boundECStates() override {
        logger::error("NullFSMState({})::boundECStates() failed. FSMState is null.", fullName());
        return {};
    }
        
    virtual std::shared_ptr<OperationInletAPI> inlet() override {
        logger::error("NullFSMState({})::inlet() called.", fullName());
        return nullOperationInlet();
    }

};


std::shared_ptr<FSMStateAPI> nerikiri::nullFSMState() { 
    return std::make_shared<NullFSMState>();
}