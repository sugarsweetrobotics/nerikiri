#pragma once

#include <string>
#include <map>

#include "nerikiri/nerikiri.h"
#include "nerikiri/object.h"



namespace nerikiri {

    class NK_API FSM : public Object {
    private:
        
    public:
        FSM(const Value& info);
        FSM();


        virtual ~FSM();

    private:
        bool _isTransitable(const std::string& current, const std::string& next);

    public:

        Value getFSMState() const;
        
        Value setFSMState(const std::string& state);

    };


    class FSMFactory : public Object {
    protected:
        FSMFactory(bool dmy) : Object() {} // For Null
    public:
        FSMFactory(const Value& info) : Object(info) {}
        FSMFactory() : Object({{"typeName", "GenericFSM"}}) {}
        virtual ~FSMFactory() {}

    public:
        virtual std::shared_ptr<FSM> create(const Value& info) {
            return std::make_shared<FSM>(info);
        }
    };

    class NullFSMFactory : public FSMFactory {
    public:

        NullFSMFactory() : FSMFactory(false) {}
        virtual ~NullFSMFactory() {}

    public:
        virtual std::shared_ptr<FSM> create(const Value& info) {
            return std::make_shared<FSM>();
        }
    };
}