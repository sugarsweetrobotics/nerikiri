#pragma once

#include <thread>
#include <chrono>

#include "nerikiri/object.h"
#include "nerikiri/operation.h"

namespace nerikiri {


    class ExecutionContext : public Object {
    private:
        std::vector<std::shared_ptr<OperationBase>> operations_;
        std::vector<std::pair<Value, std::shared_ptr<BrokerAPI>>> operationBrokers_;

    public:
        ExecutionContext(const Value& info) : Object(info) {
            info_["state"] = Value("stopped");
        }

        ExecutionContext() : Object() {}
        virtual ~ExecutionContext() {
            operations_.clear();
            operationBrokers_.clear();
        }

        static std::shared_ptr<ExecutionContext> null;

    public:
        virtual bool start() {
            if (info_["state"].stringValue() != "started") {
                onStarting();
                info_["state"] = Value("started");
                onStarted();
            }
            return true;
        }

        virtual bool stop() {
            if (info_["state"].stringValue() != "stopped") {
                onStopping();
                info_["state"] = Value("stopped");
                onStopped();
            }
            return true;
        }

        virtual bool onStarting() {return true;};
        virtual bool onStarted() {return true;}

        virtual bool onStopping() {return true;};
        virtual bool onStopped() {return true;}

        virtual void svc() {
            for(auto& op : operations_) {
                try {
                    op->execute();
                } catch (std::exception& ex) {
                    logger::error("Exception in ExecutionContext::svc(): {}", ex.what());
                }
            }
            for(auto& b : operationBrokers_) {
                try {
                    b.second->executeOperation(b.first);
                } catch (std::exception& ex) {
                    logger::error("Exception in ExecutionContext::svc(): {}", ex.what());
                }
            }
        }

        Value bind(std::shared_ptr<OperationBase> op) {
            if (op->isNull()) {
                return Value::error(logger::error("ExecutionContext::bind failed. Operation is null"));
            }
            auto info = op->info();           
            for(auto it = operations_.begin(); it != operations_.end();++it) {
                if ((*it)->info().at("instanceName") == info.at("instanceName")) {
                    it = operations_.erase(it);
                 }
            }
            for(auto it = operationBrokers_.begin(); it != operationBrokers_.end();++it) {
                if ((*it).second->info().at("name") == info.at("name")) {
                    it = operationBrokers_.erase(it);
                } 
            }

            operations_.push_back(op);
            return op->info();
        }

        Value bind(const Value& opInfo, std::shared_ptr<BrokerAPI> br) {
            operationBrokers_.push_back({opInfo, br});
            return br->info();
        }

        Value unbind(const Value& info) {
            for(auto it = operations_.begin(); it != operations_.end();++it) {
                if ((*it)->info().at("instanceName") == info.at("instanceName")) {
                    it = operations_.erase(it);
                    return info;
                }
            }
            for(auto it = operationBrokers_.begin(); it != operationBrokers_.end();++it) {
                if ((*it).second->info().at("name") == info.at("name")) {
                    it = operationBrokers_.erase(it);
                    return info;
                } 
            }
            return Value::error(logger::error("ExecutionContext::unbind({}) failed. Not Found.", str(info)));
        }

        Value getBoundOperationInfos() {
            return nerikiri::map<Value, std::shared_ptr<OperationBase>>(operations_,
              [](auto op) { return op->info(); });
        }

        std::shared_ptr<OperationBase> getBoundOperation(const Value& info) const {
            for(auto& op : operations_) {
                if (op->info().at("instanceName") == info.at("instanceName")) {
                    return op;
                }
            }
            return OperationBase::null;
        }
    };
    
    class ExecutionContextFactory {
    public:
        virtual ~ExecutionContextFactory() {}

    public:
        virtual std::shared_ptr<ExecutionContext> create(const Value& arg) const = 0;
        virtual std::string typeName() const = 0;
    };


}