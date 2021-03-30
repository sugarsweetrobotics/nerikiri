#pragma once

#include <nerikiri/nerikiri.h>// 
#include <nerikiri/object.h>
#include <nerikiri/operation_api.h>
#include <nerikiri/functional.h>
#include "nerikiri/operation/connection_container.h"
#include <nerikiri/operation_api.h>
#include <nerikiri/fsm_state_api.h>

namespace nerikiri {

        
    std::shared_ptr<OperationAPI> operationWithArgument(const std::shared_ptr<OperationAPI>& op, const Value& v);

    class FSMStateInlet : public OperationInletAPI {
    private:

        ConnectionContainer connections_;
        const std::string name_;
        std::string ownerFullName_;
    public:
        FSMStateInlet(const std::string& fullName) : OperationInletAPI() , name_(fullName), ownerFullName_("null") {}
        virtual ~FSMStateInlet() {}

        // irtual OperationAPI* owner() override { return operation_; }

        virtual std::string ownerFullName() const override {
            return ownerFullName_;
        }

        void setOwnerFullName(const std::string& ownerFullName) { ownerFullName_ = ownerFullName; }

        virtual Value executeOwner() override { 
            // TODO: Not Impl
        }

        virtual bool isNull() const override {
            return false;
        }
        
        virtual std::string name() const override { return name_; }

        virtual Value defaultValue() const override { return {}; }

        virtual Value get() const override { return {}; }

        virtual Value info() const override {

            // TODO: Not Impl
        }

        virtual Value put(const Value& value) override {

            // TODO: Not Impl
        }

        virtual Value execute(const Value& value){
            // TODO: Not Impl
        }

        virtual Value collectValues() { return {}; }

        virtual bool isUpdated() const override { return false; }

        virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override { return connections_.connections(); }

        virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& con) override {
        return connections_.addConnection(con);
        }

        virtual Value connectTo(const std::shared_ptr<OperationOutletAPI>& outlet, const Value& connectionInfo) override {
            // TODO: Not Impl
        }

        virtual Value disconnectFrom(const std::shared_ptr<OperationOutletAPI>& outlet) override {
            // TODO: Not Impl
        }
        virtual Value removeConnection(const std::string& _fullName) override {
        return connections_.removeConnection(_fullName);
        }

        
    };

    class NK_API FSMState : public FSMStateAPI {
    private:
        std::vector<std::shared_ptr<OperationAPI>> operations_;
        std::vector<std::shared_ptr<ECStateAPI>> ecStates_;
        //std::vector<std::pair<std::shared_ptr<OperationAPI>, Value>> operationAndArgs_;
        std::vector<std::string> transitableStateNames_;
        std::shared_ptr<FSMStateInlet> inlet_;
        FSMAPI* owner_;
        bool is_active_;

        //std::shared_ptr<OperationInletAPI> inlet_;
    public:
        FSMState() : is_active_(false), inlet_(std::make_shared<FSMStateInlet>("")), owner_(nullptr) {}

        FSMState(const std::string& name, const std::vector<std::string>& transitableNames = {}, const bool is_active=false) : FSMStateAPI("FSMState", "FSMState", name), transitableStateNames_(transitableNames),
          inlet_(std::make_shared<FSMStateInlet>(name)), is_active_(is_active)  {}

        //FSMState(const std::string& name): FSMState(name, {}) {}

        virtual ~FSMState() {}

        void setOwner(FSMAPI* owner) {
             owner_ = owner; 
             inlet_->setOwnerFullName(owner->fullName());
        }

    public:
        virtual Value info() const override {
            auto v = Object::info();
            v["boundOperations"] = nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(operations_, [](auto op) {
                return op->info();
            });
            v["boundECStates"] = nerikiri::functional::map<Value, std::shared_ptr<ECStateAPI>>(ecStates_, [](auto op) {
                return op->info();
            });
            return v;
        }

    //private:

        // virtual bool _setActive() override { is_active_ = true; return true; }
        
    public:

        virtual bool isActive() const override { return is_active_; }

        virtual bool activate() override {
            for(auto op : boundOperations()) {
                op->execute();
            }
            for(auto ec : boundECStates()) {
                ec->activate();
            }
            owner_->currentFsmState()->deactivate();
            is_active_ = true;
            return true;
        }

        virtual bool deactivate() override {
            is_active_ = false;
            return true;
        }

        virtual bool isTransitable(const std::string& stateName) const override {
            return std::find(transitableStateNames_.begin(), transitableStateNames_.end(), stateName) != transitableStateNames_.end();
        }

        virtual Value bind(const std::shared_ptr<OperationAPI>& op) override {
            if (op->isNull()) return Value::error(logger::error("FSMState({})::bind(OperationAPI) failed. Operation is null.", fullName()));
            return operations_.emplace_back(op) -> info();
        }

        virtual Value bind(const std::shared_ptr<OperationAPI>& op, const Value& arg) override {
            if (op->isNull()) return Value::error(logger::error("FSMState({})::bind(OperationAPI) failed. Operation is null.", fullName()));
            //operationAndArgs_.push_back({op, arg});
            return operations_.emplace_back(operationWithArgument(op, arg))->info();
        }

        virtual Value bind(const std::shared_ptr<ECStateAPI>& ecs) override {
            if (ecs->isNull()) return Value::error(logger::error("FSMState({})::bind(ECStateAPI) failed. ECState is null.", fullName()));
            return ecStates_.emplace_back(ecs) -> info();
        }

        virtual Value unbind(const std::shared_ptr<OperationAPI>& op) override {
            if (op->isNull()) return Value::error(logger::error("FSMState({})::unbind(OperationAPI) failed. Operation is null.", fullName()));
            operations_.erase(std::remove(operations_.begin(), operations_.end(), op), operations_.end());
            return op->info();
        }

        virtual Value unbind(const std::shared_ptr<ECStateAPI>& e) override {
            if (e->isNull()) return Value::error(logger::error("FSMState({})::unbind(ECStateAPI) failed. ECState is null.", fullName()));
            ecStates_.erase(std::remove(ecStates_.begin(), ecStates_.end(), e), ecStates_.end());
            return e->info();
        }

        virtual std::shared_ptr<OperationInletAPI> inlet() override {
            return inlet_;
        }

        virtual std::vector<std::shared_ptr<OperationAPI>> boundOperations() override { return operations_; }

        virtual std::vector<std::shared_ptr<ECStateAPI>> boundECStates() override { return ecStates_; }
    };

}