#pragma once

#include <thread>
#include <chrono>

#include "nerikiri/object.h"
#include "nerikiri/logger.h"
#include "nerikiri/operationbase.h"
#include "nerikiri/naming.h"

namespace nerikiri {

#if 0
    class FSMObject;

    class State : public Object {
    private:
        FSMObject* owner_;
    public:
        State(const std::string& name, FSMObject* owner): Object({{"stateName", name}}), owner_(owner) {}
        virtual ~State() {}

    public:
        std::string stateName() const { return info().at("stateName").stringValue(); }

        Value activate() {
            return owner_->activateState(this);
        }
    };


    class FSMObject: public Object {
    private:
        std::vector<State> states_;
    public:
        FSMObject(const Value& info): Object(info) {}

        virtual ~FSMObject() {}

    public:
        void addState(State& state) {
            for(auto it = states_.begin(); it != states_.end(); ++it) {
                if ((*it).stateName() == state.stateName()) {
                    states_.erase(it);
                    return;
                }   
            }

            states_.push_back(state);
        }

        void addDefaultState(State& state) {
            for(auto it = states_.begin(); it != states_.end(); ++it) {
                if ((*it).stateName() == state.stateName()) {
                    states_.erase(it);
                    return;
                }   
            }

            states_.push_back(state);
        }

        void deleteState(State& state) {
            for(auto it = states_.begin(); it != states_.end(); ++it) {
                if ((*it).stateName() == state.stateName()) {
                    states_.erase(it);
                    return;
                }   
            }
        }
    public:
        Value activateState(const State* state) {
            for(auto it = states_.begin(); it != states_.end(); ++it) {
                if ((*it).stateName() == state->stateName()) {

        }
    };

#endif
    class ExecutionContext : public Object {
    private:
        std::vector<std::shared_ptr<OperationBase>> operations_;
        std::vector<std::pair<std::string, std::shared_ptr<BrokerAPI>>> operationBrokers_;

    public:
        ExecutionContext(const Value& info) : Object(info) {
            info_["state"] = Value("stopped");
        }

        ExecutionContext() : Object() {}
        virtual ~ExecutionContext() {
            operations_.clear();
            operationBrokers_.clear();
        }


    public:

        virtual Value setState(const std::string& state) { 
            if (state == "started") {
                return start();
            } else if (state == "stopped") {
                return stop();
            } 
            return Value::error(logger::error("ExecutionContext({})::setState({}) failed. Unknown State.", getInstanceName(), state));
        }

        virtual Value start() {
            logger::trace("ExecutionContext({})::start() called", getInstanceName());
            if (info_["state"].stringValue() != "started") {
                onStarting();
                info_["state"] = Value("started");
                if (!onStarted()) {
                    stop();
                    return Value::error(logger::error("ExecutionContext::start() failed. onStarted callback returns false."));;
                }
                logger::info("ExecutionContext({}) successfully started.", getInstanceName());
            }
            return info();
        }

        virtual Value stop() {
            logger::trace("ExecutionContext({})::stop() called", getInstanceName());
            if (info_["state"].stringValue() != "stopped") {
                onStopping();
                info_["state"] = Value("stopped");
                onStopped();
                logger::info("ExecutionContext({}) successfully stopped.", getInstanceName());
            }
            return info();
        }

        virtual bool onStarting() {return true;};
        virtual bool onStarted() {return true;}

        virtual bool onStopping() {return true;};
        virtual bool onStopped() {return true;}

        virtual void svc() {
            logger::trace("ExecutionContext::svc()");
            for(auto& op : operations_) {
                try {
                    logger::trace("In EC::svc. OperationBase({})::executing....", op->info().at("fullName"));
                    auto v = op->execute();
                    logger::trace("In EC::svc. execution result is {}", v);
                } catch (std::exception& ex) {
                    logger::error("In EC::svc. Exception in ExecutionContext::svc(): {}", ex.what());
                }
            }
            for(auto& b : operationBrokers_) {
                try {
                    logger::trace("In EC::svc. Broker()::executing Operation({})....", b.first);
                    auto v = b.second->executeOperation(b.first);
                    logger::trace("In EC::svc. execution result is {}", v);
                } catch (std::exception& ex) {
                    logger::error("In EC::svc. Exception in ExecutionContext::svc(): {}", ex.what());
                }
            }
        }

        Value bind(std::shared_ptr<OperationBase> op) {
            if (op->isNull()) {
                return Value::error(logger::error("ExecutionContext::bind failed. Operation is null"));
            }
            auto info = op->info();     
            for(auto it = operations_.begin(); it != operations_.end();++it) {
                if ((*it)->info().at("fullName") == info.at("fullName")) {
                    it = operations_.erase(it);
                }
            }
            for(auto it = operationBrokers_.begin(); it != operationBrokers_.end();++it) {
                if ((*it).second->info().at("fullName") == info.at("fullName")) {
                    it = operationBrokers_.erase(it);
                } 
            }
            operations_.push_back(op);
            return op->info();
        }

        Value bind(const std::string& opName, std::shared_ptr<BrokerAPI> br) {
            /// 存在確認
            //if (opInfo.at("fullName").stringValue().find(":") >= 0) {
            //    auto instanceName = opInfo.at("fullName").stringValue();
            //    auto containerName = instanceName.substr(0, instanceName.find(":"));
            //    auto operationName = instanceName.substr(instanceName.find(":")+1);
            //    auto i = br->getContainerOperationInfo({{"fullName", containerName}}, {{"fullName", operationName}});
            //    if (i.at("fullName").stringValue() == "null") return i;
            //} else {
            auto i = br->getOperationInfo(opName);
            if (i.at("fullName").stringValue() == "null") return i;
            //}
            operationBrokers_.push_back({opName, br});
            return br->info();
        }

        Value unbind(const std::string& opName) {
            
            for(auto it = operations_.begin(); it != operations_.end();++it) {
                if ((*it)->info().at("fullName").stringValue() == opName) {
                    it = operations_.erase(it);
                    return opName;
                } 
            }
            
            for(auto it = operationBrokers_.begin(); it != operationBrokers_.end();++it) {
                if ((*it).first == opName) {
                    auto br = it->second->info();
                    it = operationBrokers_.erase(it);
                    return br;
                } 
            }
            return Value::error(logger::error("ExecutionContext::unbind({}) failed. Not Found.", opName));
        }

        Value getBoundOperationInfos() {
            auto ops = nerikiri::map<Value, std::shared_ptr<OperationBase>>(operations_,
              [](auto op) { return op->info(); });
            for(auto p : operationBrokers_) {
                Value info = p.second->getOperationInfo(p.first);
                info["broker"] = p.second->info();
                ops.push_back(info);
            }
            return ops;
        }

        std::shared_ptr<OperationBase> getBoundOperation(const Value& info) const {
            for(auto& op : operations_) {
                if (op->info().at("fullName") == info.at("fullName")) {
                    return op;
                }
            }
            return nullOperation();
        }
    };
    

    inline std::shared_ptr<ExecutionContext> nullExecutionContext() {
      return std::make_shared<ExecutionContext>();
    }

    class ExecutionContextFactory {
    public:
        virtual ~ExecutionContextFactory() {}

    public:
        virtual std::shared_ptr<ExecutionContext> create(const Value& arg) const = 0;
        virtual std::string typeName() const = 0;
    };

    template<typename T>
    class ECFactory : public ExecutionContextFactory {
    private:
        const std::string typeName_;
    public:
        ECFactory() : typeName_(nerikiri::demangle(typeid(T).name())) {}
        virtual ~ECFactory() {}

    public:
        virtual std::shared_ptr<ExecutionContext> create(const Value& arg) const {
            return std::shared_ptr<ExecutionContext>(new T(arg));
        };

        virtual std::string typeName() const { 
            return typeName_;
        }
    };


}