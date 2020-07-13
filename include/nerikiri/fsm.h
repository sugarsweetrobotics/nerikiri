#pragma once

#include <string>
#include <map>
#include <vector>

#include "nerikiri/nerikiri.h"
#include "nerikiri/object.h"
#include "nerikiri/operationbase.h"



namespace nerikiri {

    class NK_API FSM : public Object {
    private:
        std::vector<Connection> connections_;
        std::string currentStateName_;
        std::map<std::string, std::vector<std::shared_ptr<OperationBase>>> operations_;
        std::map<std::string, std::vector<std::pair<std::string, std::shared_ptr<BrokerAPI>>>> operationBrokers_;
    public:
        FSM(const Value& info);
        FSM();

        virtual ~FSM();

    private:
        bool _isTransitable(const std::string& current, const std::string& next);

    public:

        Value getFSMState() const;
        
        Value setFSMState(const std::string& state);

        Value getFSMStates() const;

        bool hasState(const std::string& stateName) const;

        Value bindStateToOperation(const std::string& stateName, const std::shared_ptr<OperationBase>& op);

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

    private:
        Value _executeInState(const std::string& stateName);
    };

    inline static std::shared_ptr<FSM> nullFSM() {
      return std::make_shared<FSM>();
    }

    class FSMFactory : public Object {
    protected:
        FSMFactory(bool dmy) : Object() {} // For Null
    public:
        FSMFactory(const Value& info) : Object(info) {}
        FSMFactory() : Object({{"typeName", "GenericFSM"}}) {}
        virtual ~FSMFactory() {}

    public:
        virtual std::shared_ptr<FSM> create(const Value& info) {
            return std::make_shared<FSM>(info);
        }
    };

    class NullFSMFactory : public FSMFactory {
    public:

        NullFSMFactory() : FSMFactory(false) {}
        virtual ~NullFSMFactory() {}

    public:
        virtual std::shared_ptr<FSM> create(const Value& info) {
            return std::make_shared<FSM>();
        }
    };
}