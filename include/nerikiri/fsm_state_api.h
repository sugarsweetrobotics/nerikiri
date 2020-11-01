#pragma once



namespace nerikiri {

    class FSMAPI;

    class NK_API FSMStateAPI : public Object {
    public:
        FSMStateAPI() {}
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

        virtual Value unbind(const std::shared_ptr<OperationAPI>& op) = 0;

        virtual std::vector<std::shared_ptr<OperationAPI>> boundOperations() = 0;

    protected:
        virtual bool _setActive() = 0;

        friend class FSMAPI;
    };


    class NK_API NullFSMState : public FSMStateAPI {
    public:
        NullFSMState() {}
        virtual ~NullFSMState() {}

    public:

        virtual bool isActive() const override {
            logger::error("NullFSMState()::isActive() failed. FSMState is null.");
            return false;
        }

        virtual bool activate() override {
            logger::error("NullFSMState()::activate() failed. FSMState is null.");
            return false;
        }

        virtual bool deactivate() override {
            logger::error("NullFSMState()::{}() failed. FSMState is null.", __FUNCTION__);
            return false;
        }


        virtual bool _setActive() override {
            logger::error("NullFSMState()::{}() failed. FSMState is null.", __FUNCTION__);
            return false;
        }


        virtual bool isTransitable(const std::string& stateName) const override {
            logger::error("NullFSMState()::isTransitable(string stateName({})) failed. FSMState is null.", stateName);
            return false;
        }

        virtual Value bind(const std::shared_ptr<OperationAPI>& op) override {
            return Value::error(logger::error("NullFSMState()::bind(OperationAPI op({})) failed. FSMState is null.", op->fullName()));
        }

        virtual Value bind(const std::shared_ptr<OperationAPI>& op, const Value& arg) override {
            return Value::error(logger::error("NullFSMState()::bind(OperationAPI op({})) failed. FSMState is null.", op->fullName()));
        }

        virtual Value unbind(const std::shared_ptr<OperationAPI>& op) override {
            return Value::error(logger::error("NullFSMState()::unbind(OperationAPI op({})) failed. FSMState is null.", op->fullName()));
        }

        virtual std::vector<std::shared_ptr<OperationAPI>> boundOperations() override {
            logger::error("NullFSMState({})::boundOperations() failed. FSMState is null.");
            return {};
        }
        
    };
}