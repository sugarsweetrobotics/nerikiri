#pragma once

#include <thread>
#include <chrono>

#include "nerikiri/object.h"
#include "nerikiri/logger.h"
#include "nerikiri/operationbase.h"

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


    public:
        virtual Value start() {
            if (info_["state"].stringValue() != "started") {
                onStarting();
                info_["state"] = Value("started");
                if (!onStarted()) {
                    stop();
                    return Value::error("ExecutionContext::start() failed. onStarted callback returns false.");;
                }
            }
            return info();
        }

        virtual Value stop() {
            if (info_["state"].stringValue() != "stopped") {
                onStopping();
                info_["state"] = Value("stopped");
                onStopped();
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
                    logger::trace("In EC::svc. Broker()::executing Operation({})....", b.first.at("fullName"));
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
                if ((*it).second->info().at("name") == info.at("name")) {
                    it = operationBrokers_.erase(it);
                } 
            }

            operations_.push_back(op);
            return op->info();
        }

        Value bind(const Value& opInfo, std::shared_ptr<BrokerAPI> br) {
            /// 存在確認
            //if (opInfo.at("fullName").stringValue().find(":") >= 0) {
            //    auto instanceName = opInfo.at("fullName").stringValue();
            //    auto containerName = instanceName.substr(0, instanceName.find(":"));
            //    auto operationName = instanceName.substr(instanceName.find(":")+1);
            //    auto i = br->getContainerOperationInfo({{"fullName", containerName}}, {{"fullName", operationName}});
            //    if (i.at("fullName").stringValue() == "null") return i;
            //} else {
                auto i = br->getOperationInfo(opInfo);
                if (i.at("fullName").stringValue() == "null") return i;
            //}
            operationBrokers_.push_back({opInfo, br});
            return br->info();
        }

        Value unbind(const Value& info) {
            for(auto it = operations_.begin(); it != operations_.end();++it) {
                if ((*it)->info().at("fullName") == info.at("fullName")) {
                    it = operations_.erase(it);
                    return info;
                } 
            }
            for(auto it = operationBrokers_.begin(); it != operationBrokers_.end();++it) {
                if ((*it).first.at("fullName") == info.at("fullName")) {
                    it = operationBrokers_.erase(it);
                    return info;
                } 
            }
            return Value::error(logger::error("ExecutionContext::unbind({}) failed. Not Found.", str(info)));
        }

        Value getBoundOperationInfos() {
            auto ops = nerikiri::map<Value, std::shared_ptr<OperationBase>>(operations_,
              [](auto op) { return op->info(); });
            for(auto p : operationBrokers_) {
                Value info = p.first;
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


}