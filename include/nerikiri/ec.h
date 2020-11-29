#pragma once

#include <thread>
#include <chrono>
#include <vector>
#include <memory>

#include <nerikiri/object.h>
#include <nerikiri/logger.h>
#include <nerikiri/operation_api.h>

#include <nerikiri/ec_api.h>
#include <nerikiri/ec_factory_api.h>

#include <nerikiri/functional.h>


namespace nerikiri {

    class ExecutionContextBase : public ExecutionContextAPI {
    private:
        std::vector<std::shared_ptr<OperationAPI>> operations_;
        std::shared_ptr<ECStateAPI> startedState_;
        std::shared_ptr<ECStateAPI> stoppedState_;
    public:
        ExecutionContextBase(const std::string& typeName, const std::string& fullName);
        virtual ~ExecutionContextBase();


    public:
        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override { return operations_; }

        virtual Value start() override;

        virtual Value stop() override ;

        virtual bool onStarting() {return true;};
        virtual bool onStarted() {return true;}

        virtual bool onStopping() {return true;};
        virtual bool onStopped() {return true;}

        virtual void svc();

        virtual Value bind(const std::shared_ptr<OperationAPI>& op) override;

        virtual Value unbind(const std::string& opName) override;

        std::shared_ptr<OperationAPI> operation(const std::string& fullName) const;


        virtual std::shared_ptr<ECStateAPI> startedState() override { return startedState_; }

        virtual std::shared_ptr<ECStateAPI> stoppedState() override { return stoppedState_; }
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
        virtual std::shared_ptr<ExecutionContextAPI> create(const std::string& fullName) const override {
            return std::shared_ptr<ExecutionContextAPI>(new T(fullName));
        };
    };


}