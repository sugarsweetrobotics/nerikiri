#pragma once
#include <string>
#include <vector>

#include <nerikiri/object.h>
#include <nerikiri/fsm_api.h>
namespace nerikiri {

    class FSMFactoryAPI : public Object {
    protected:
        FSMFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {} // For Null
    public:
        virtual ~FSMFactoryAPI() {}

    public:

      virtual std::shared_ptr<FSMAPI> create(const std::string& _fullName, const Value& extInfo) const = 0;
    };


    std::shared_ptr<FSMFactoryAPI> nullFSMFactory();

}