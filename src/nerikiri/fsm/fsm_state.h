#pragma once

#include <nerikiri/nerikiri.h>// 
#include <nerikiri/object.h>
#include <nerikiri/operation_api.h>
#include <nerikiri/functional.h>
#include <nerikiri/operation_api.h>
#include <nerikiri/fsm_state_api.h>

namespace nerikiri {

        
    std::shared_ptr<OperationAPI> operationWithArgument(const std::shared_ptr<OperationAPI>& op, const Value& v);


    class NK_API FSMState : public FSMStateAPI {
    private:
        std::vector<std::shared_ptr<OperationAPI>> operations_;
        //std::vector<std::pair<std::shared_ptr<OperationAPI>, Value>> operationAndArgs_;
        std::vector<std::string> transitableStateNames_;

        bool is_active_;
    public:
        FSMState() : is_active_(false) {}

        FSMState(const std::string& name, const std::vector<std::string>& transitableNames = {}) : FSMStateAPI(Value{
            {
                {"instanceName", name},
                {"fullName", name},
                {"typeName", "FSMState"}
            }
        }), is_active_(false), transitableStateNames_(transitableNames)  {}

        //FSMState(const std::string& name): FSMState(name, {}) {}

        virtual ~FSMState() {}

    public:
        virtual Value info() const override {
            auto v = Object::info();
            v["boundOperations"] = nerikiri::functional::map<Value, std::shared_ptr<OperationAPI>>(operations_, [](auto op) {
                return op->info();
            });
            return v;
        }

    private:

        virtual bool _setActive() override {
            is_active_ = true;
            return true;
        }
        
    public:

        virtual bool isActive() const override {
            logger::error("NullFSMState()::isActive() failed. FSMState is null.");
            return false;
        }



        virtual bool activate() override {
            for(auto op : boundOperations()) {
                op->execute();
            }
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

        virtual Value unbind(const std::shared_ptr<OperationAPI>& op) override {
            if (op->isNull()) return Value::error(logger::error("FSMState({})::unbind(OperationAPI) failed. Operation is null.", fullName()));
            operations_.erase(std::remove(operations_.begin(), operations_.end(), op), operations_.end());
            return op->info();
        }

        virtual std::vector<std::shared_ptr<OperationAPI>> boundOperations() override { return operations_; }

    };

}