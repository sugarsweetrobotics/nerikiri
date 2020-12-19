#pragma once

#include <nerikiri/object.h>
//#include <nerikiri/operation_api.h>
//#include <nerikiri/ec_state_api.h>

namespace nerikiri {

    class OperationAPI;
    class OperationInletAPI;
    class ECStateAPI;
    class FSMAPI;

    class NK_API FSMStateAPI : public Object {
    public:
        FSMStateAPI() {}
        FSMStateAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}
        FSMStateAPI(const Value& value) : Object(value) {}
        virtual ~FSMStateAPI() {}

    public:

    public:

        virtual bool isActive() const = 0;

        virtual bool activate() = 0;

        virtual bool deactivate() = 0;

        virtual bool isTransitable(const std::string& stateName) const = 0;

        virtual Value bind(const std::shared_ptr<OperationAPI>& op) = 0;

        virtual Value bind(const std::shared_ptr<OperationAPI>& op, const Value& arg) = 0;

        virtual Value bind(const std::shared_ptr<ECStateAPI>& ecs) = 0;

        virtual Value unbind(const std::shared_ptr<OperationAPI>& op) = 0;

        virtual Value unbind(const std::shared_ptr<ECStateAPI>& ecs) = 0;

        virtual std::vector<std::shared_ptr<OperationAPI>> boundOperations() = 0;

        virtual std::vector<std::shared_ptr<ECStateAPI>> boundECStates() = 0;

        virtual std::shared_ptr<OperationInletAPI> inlet() = 0;
    protected:
        // virtual bool _setActive() = 0;

        friend class FSMAPI;
    };


    
    std::shared_ptr<FSMStateAPI> nullFSMState();
}