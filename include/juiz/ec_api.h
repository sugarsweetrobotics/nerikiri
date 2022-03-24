#pragma once

#include <memory>
#include <juiz/object.h>
#include <juiz/operation_api.h>


namespace juiz {

    class ProcessStore;

    class ExecutionContextAPI {
    private:
    public:
        ExecutionContextAPI() {}
        virtual ~ExecutionContextAPI() {}

        virtual bool onStarting() = 0;
        virtual bool onStarted() = 0;

        virtual bool onStopping() = 0;
        virtual bool onStopped() = 0;

        virtual bool svc() = 0;

        virtual void setSvcOperation(const std::shared_ptr<OperationAPI>& op) = 0;

        virtual bool isNull() const { return false; }

    };

    std::shared_ptr<ExecutionContextAPI> nullEC();

    template<>
    inline std::shared_ptr<ExecutionContextAPI> nullObject<ExecutionContextAPI>() { return nullEC(); }

    /**
     * 
     */
    class ExecutionContextFactoryAPI : public Object {
    public:
        ExecutionContextFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName)
         : Object(className, typeName, fullName) {}
        virtual ~ExecutionContextFactoryAPI() {}

        /**
         * 
         */
        virtual std::shared_ptr<ExecutionContextAPI> create(const Value& value) const = 0;
    };

    /**
     * 
     */
    std::shared_ptr<ExecutionContextFactoryAPI> nullECFactory();



    Value bind(ProcessStore& store, const Value& ecInfo, const Value& opInfo);

}