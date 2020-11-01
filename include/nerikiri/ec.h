#pragma once

#include <thread>
#include <chrono>
#include <vector>
#include <memory>

#include <nerikiri/object.h>
#include <nerikiri/logger.h>
#include <nerikiri/operation_api.h>

#include <nerikiri/ec_api.h>
#include <nerikiri/functional.h>


namespace nerikiri {

    class ExecutionContextBase : public ExecutionContextAPI {
    private:
        std::vector<std::shared_ptr<OperationAPI>> operations_;

    public:
        ExecutionContextBase(const std::string& typeName, const std::string& fullName) : 
          ExecutionContextAPI(typeName, fullName) {
        }

        virtual ~ExecutionContextBase() { }


    public:
        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override { return operations_; }

        virtual Value start() override {
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

        virtual Value stop() override {
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
        }

        virtual Value bind(const std::shared_ptr<OperationAPI>& op) override {
            if (op->isNull()) {
                return Value::error(logger::error("ExecutionContext::bind failed. Operation is null"));
            }
            auto info = op->info();     
            for(auto it = operations_.begin(); it != operations_.end();++it) {
                if ((*it)->info().at("fullName") == info.at("fullName")) {
                    it = operations_.erase(it);
                }
            }
            operations_.push_back(op);
            return op->info();
        }

        virtual Value unbind(const std::string& opName) override {
            
            for(auto it = operations_.begin(); it != operations_.end();++it) {
                if ((*it)->info().at("fullName").stringValue() == opName) {
                    it = operations_.erase(it);
                    return opName;
                } 
            }
            return Value::error(logger::error("ExecutionContext::unbind({}) failed. Not Found.", opName));
        }

        std::shared_ptr<OperationAPI> operation(const std::string& fullName) const { 
            auto op = nerikiri::functional::find<std::shared_ptr<OperationAPI>>(operations(), [&fullName](auto op) { return op->fullName() == fullName; });
            if (op) return op.value();
            return std::make_shared<NullOperation>();
        }
    };

    class ExecutionContextFactoryAPI : public Object {
    public:
        ExecutionContextFactoryAPI(const std::string& typeName, const std::string& fullName) :
          Object(typeName, fullName) {}
        virtual ~ExecutionContextFactoryAPI() {}

        virtual std::shared_ptr<ExecutionContextAPI> create(const std::string& fullName) = 0;
        virtual std::string executionContextTypeFullName() const = 0;
    };

    class NullExecutionContextFactory : public ExecutionContextFactoryAPI{
    public:
        NullExecutionContextFactory() :
          ExecutionContextFactoryAPI("NullExecutionContextFactory", "null") {}
        virtual ~NullExecutionContextFactory() {}

        virtual std::string executionContextTypeFullName() const { return "NullExecutionContext"; }

        virtual std::shared_ptr<ExecutionContextAPI> create(const std::string& fullName) {
            logger::warn("NullExecutionContextFactory::create() called. ExecutionContextFactory is null.");
            return std::make_shared<NullExecutionContext>();
        }
      
    };

    class ExecutionContextFactoryBase : public ExecutionContextFactoryAPI {
    private:
    private:
        const std::string executionContextTypeFullName_;
    
    public:
        ExecutionContextFactoryBase(const std::string& typeName, const std::string& executionContextTypeFullName, const std::string& fullName) :
          ExecutionContextFactoryAPI(typeName, fullName), executionContextTypeFullName_(executionContextTypeFullName) {}
        virtual ~ExecutionContextFactoryBase() {}

        virtual std::string executionContextTypeFullName() const { return executionContextTypeFullName_; }
    public:
    };

    template<typename T>
    class ECFactory : public ExecutionContextFactoryBase {
    public:
        ECFactory(const std::string& fullName) : ExecutionContextFactoryBase("ECFactory", nerikiri::demangle(typeid(T).name()), fullName) {}
        virtual ~ECFactory() {}

    public:
        virtual std::shared_ptr<ExecutionContextAPI> create(const std::string& fullName) const {
            return std::shared_ptr<ExecutionContextAPI>(new T(fullName));
        };
    };


}