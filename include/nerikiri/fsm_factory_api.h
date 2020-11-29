#pragma once
#include <string>
#include <vector>

#include <nerikiri/object.h>

namespace nerikiri {

    class FSMFactoryAPI : public Object {
    protected:
        FSMFactoryAPI(const std::string& typeName, const std::string& fullName) : Object(typeName, fullName) {} // For Null
    public:
        virtual ~FSMFactoryAPI() {}

    public:
      virtual std::string fsmTypeFullName() const = 0;

      virtual std::shared_ptr<FSMAPI> create(const std::string& _fullName) const = 0;
    };


    class NullFSMFactory : public FSMFactoryAPI {
    public:

        NullFSMFactory() : FSMFactoryAPI("NullFSMFactory", "null") {}
        virtual ~NullFSMFactory() {}

      virtual std::string fsmTypeFullName() const override { return "NullFSM"; }

    public:
        virtual std::shared_ptr<FSMAPI> create(const std::string& _fullName) const override {
            return std::make_shared<NullFSM>();
        }
    };
}