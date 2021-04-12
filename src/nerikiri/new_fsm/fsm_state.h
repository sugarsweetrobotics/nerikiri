#pragma once

#include <nerikiri/nerikiri.h>
#include <nerikiri/object.h>
#include <nerikiri/operation_api.h>
#include <nerikiri/functional.h>
#include "nerikiri/operation/connection_container.h"
#include <nerikiri/operation_api.h>
#include <nerikiri/fsm_state_api.h>

namespace nerikiri {

    class NK_API FSMState : public FSMStateAPI {
    private:
        const FSMAPI* owner_;
        const std::shared_ptr<OperationAPI> operation_;
        std::vector<std::string> transitableStateNames_;
    public:
        FSMState(FSMAPI* owner, const std::string& name, const std::shared_ptr<OperationAPI>& operation, const std::vector<std::string>& transitableNames = {}, const bool is_active=false);

        virtual ~FSMState();

        virtual void finalize() override;

        // void setOwner(FSMAPI* owner);

    public:
        virtual Value info() const override;
    public:

        virtual bool isActive() const override;

        virtual bool activate() override;
        virtual bool deactivate() override ;

        virtual bool isTransitable(const std::string& stateName) const override;
        virtual Value bind(const std::shared_ptr<OperationAPI>& op) override;

        virtual Value bind(const std::shared_ptr<OperationAPI>& op, const Value& arg) override;

        virtual Value bind(const std::shared_ptr<ECStateAPI>& ecs) override;

        virtual Value unbind(const std::shared_ptr<OperationAPI>& op) override;

        virtual Value unbind(const std::shared_ptr<ECStateAPI>& e) override;

        virtual std::shared_ptr<OperationInletAPI> inlet() override;

        virtual std::shared_ptr<OperationOutletAPI> outlet() override;

        virtual std::vector<std::shared_ptr<OperationAPI>> boundOperations() override;

        virtual std::vector<std::shared_ptr<ECStateAPI>> boundECStates() override;
    };
}