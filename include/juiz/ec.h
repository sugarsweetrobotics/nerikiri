#pragma once
#include <memory>
#include <string>

#include <juiz/operation_api.h>
#include <juiz/logger.h>

#include <juiz/ec_api.h>

namespace juiz {

    /**
     * 実行コンテキストのベースクラス
     */
    class ExecutionContextBase : public ExecutionContextAPI {
    private:
        std::shared_ptr<OperationAPI> svcOperation_;
        const std::string& typeName_;
        std::mutex svc_mutex_;
    public:
        ExecutionContextBase(const std::string& typeName): ExecutionContextAPI(), typeName_(typeName) {}
        virtual ~ExecutionContextBase() {}

    public:
        virtual bool onStarting() override { return true; }
        virtual bool onStarted() override { return true; }
        virtual bool onStopping() override { return true; }
        virtual bool onStopped() override { return true; }

        virtual bool svc() override; 

        virtual void setSvcOperation(const std::shared_ptr<OperationAPI>& op) override { svcOperation_ = op; }
    };

    /**
     * 
     */
    template<typename T>
    class ECFactory : public ExecutionContextFactoryAPI {
    public:
        ECFactory() : ExecutionContextFactoryAPI("ECFactory", juiz::demangle(typeid(T).name()), "ECFactory:" + juiz::demangle(typeid(T).name()) + "0.ecf") {}
        virtual ~ECFactory() {}

        virtual std::shared_ptr<ExecutionContextAPI> create(const Value& value) const override {
            return std::shared_ptr<ExecutionContextAPI>(new T(value));
        }
    };


}