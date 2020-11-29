#pragma once

#include <string>
#include <map>
#include <vector>

#include "nerikiri/nerikiri.h"
#include "nerikiri/object.h"
//#include <nerikiri/operation_base.h>
#include "nerikiri/ec.h"

#include "nerikiri/fsm_state.h"
#include <nerikiri/fsm_api.h>

namespace nerikiri {

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
            if (s) s.value();
            return std::shared_ptr<NullFSMState>();
        }

        virtual std::shared_ptr<FSMStateAPI> currentFsmState() const override {
            auto s = nerikiri::functional::find<std::shared_ptr<FSMStateAPI>>(states_, [](auto s) {
                return s->isActive();
            });
            if (s) s.value();
            return std::shared_ptr<NullFSMState>();
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
        FSM(const Value& info);

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

    class FSMFactoryAPI : public Object {
    protected:
        FSMFactoryAPI(const std::string& typeName, const std::string& fullName) : Object(typeName, fullName) {} // For Null
    public:
        virtual ~FSMFactoryAPI() {}

    public:
      virtual std::string fsmTypeFullName() const = 0;

      virtual std::shared_ptr<FSMAPI> create(const std::string& _fullName) const = 0;
    };

    class FSMFactory : public FSMFactoryAPI {
    public:
        FSMFactory(const std::string& fullName) : FSMFactoryAPI("FSMFactory", fullName) {} // For Null
    public:
        virtual ~FSMFactory() {}
        virtual std::string fsmTypeFullName() const override {
            return "FSM";
        }

    public:
        virtual std::shared_ptr<FSMAPI> create(const std::string& _fullName) const override {
            return std::make_shared<FSM>(_fullName);
        }
    };

    class NullFSMFactory : public FSMFactoryAPI {
    public:

        NullFSMFactory() : FSMFactoryAPI("NullFSMFactory", "null") {}
        virtual ~NullFSMFactory() {}

      virtual std::string fsmTypeFullName() const override { return "NullFSM"; }

    public:
        virtual std::shared_ptr<FSMAPI> create(const std::string& _fullName) const override {
            return std::make_shared<NullFSM>();
        }
    };
}