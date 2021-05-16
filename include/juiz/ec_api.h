#pragma once

#include <memory>
#include <juiz/object.h>

namespace juiz {

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
    };

    std::shared_ptr<ExecutionContextAPI> nullEC();

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
}