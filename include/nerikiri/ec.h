#pragma once
#include <memory>
#include <string>

#include <nerikiri/operation_api.h>
#include <nerikiri/logger.h>

#include <nerikiri/ec_api.h>
#include <nerikiri/ec_factory_api.h>

namespace nerikiri {

    /**
     * 実行コンテキストのベースクラス
     */
    class ExecutionContextBase : public ExecutionContextAPI {
    private:
        std::shared_ptr<OperationAPI> svcOperation_;
        const std::string& typeName_;
    public:
        ExecutionContextBase(const std::string& typeName): ExecutionContextAPI(), typeName_(typeName) {}
        virtual ~ExecutionContextBase() {}

    public:
        virtual bool onStarting() override { return true; }
        virtual bool onStarted() override { return true; }
        virtual bool onStopping() override { return true; }
        virtual bool onStopped() override { return true; }

        virtual bool svc() override { 
            logger::trace("ExecutionContextBase::svc() called");
            return !svcOperation_->execute().isError(); 
        }
    };

    /**
     * 
     */
    template<typename T>
    class ECFactory : public ExecutionContextFactoryAPI {
    public:
        ECFactory() : ExecutionContextFactoryAPI("ECFactory", nerikiri::demangle(typeid(T).name()), "ECFactory:" + nerikiri::demangle(typeid(T).name()) + "0.ecf") {}
        virtual ~ECFactory() {}

        virtual std::shared_ptr<ExecutionContextAPI> create(const Value& value) const override {
            return std::shared_ptr<ExecutionContextAPI>(new T(value));
        }
    };

}