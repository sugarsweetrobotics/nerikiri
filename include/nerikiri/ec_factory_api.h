#pragma once


#include <memory>
#include <string>

#include <nerikiri/object.h>
#include <nerikiri/logger.h>
#include <nerikiri/ec_api.h>

namespace nerikiri {


    class ExecutionContextFactoryAPI : public Object {
    public:
        ExecutionContextFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName) :
          Object(className, typeName, fullName) {}
        virtual ~ExecutionContextFactoryAPI() {}

        virtual std::shared_ptr<ExecutionContextAPI> create(const Value& value) const = 0;
        virtual std::string executionContextTypeFullName() const = 0;
    };

    std::shared_ptr<ExecutionContextFactoryAPI> nullECFactory();
}