#pragma once 

#include <nerikiri/operation_api.h>
#include <nerikiri/brokerapi.h>

namespace nerikiri {


    class OperationOutletCRUDBroker : public OperationOutletAPI {
    private:
        const std::shared_ptr<CRUDBrokerAPI>> crudBroker_;

    public:
        OperationOutletCRUDBroker(const std::string& fullName, const std::shared_ptr<CRUBRokerAPI>>& crudBroker): OperationOutletAPI("OperationOutletCRUDBroker", fullName), crudBroker_(crudBroker) {}
        virtual ~OperationOutletCRUDBroker() {}

    public:
        virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const {
            auto infos = crudBroker_->readObject("/operations/" + fullName() + "/outlet/connections/");
        }

        virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) {
            return crudBroker_->createObject("/operations/" + fullName() + "/outlet/connections/", c->info());
        }
        
        virtual Value removeConnection(const std::string& _fullName) {
            return crudBroker_->deleteObject("/operations/" + fullName() + "/outlet/connections/" + _fullName());
        }
    };


    class OperationCRUDBroker : public OperationAPI {
    private:
        const std::shared_ptr<CRUDBrokerAPI>> crudBroker_;
    public:
        OperationCRUDBroker(const std::string& fullName, const std::shared_ptr<CRUDBrokerAPI>>& crudBroker) :
         OperationAPI("OperationCRUDBroker", fullName), crudBroker_(crudBroker) {}

        virtual ~OperationCRUDBroker() {}

    public:

        virtual Value info() const {
            return crudBroker_->readObject("/operations/" + fullName() + "/");
        }

        virtual std::string operationTypeName() const {
            return crudBroker_->readObject("/operations/" + fullName() + "/typeName/");
        }

        virtual Value call(const Value& value) {
            return crudBroker_->readObject("/operations/" + fullName() + "/", value);
        }

        /**
         * This function collects the value from inlets and calls the function inside the Operation,
         * but this does not execute the output side operation
         */
        virtual Value invoke() {
            return crudBroker_->updateObject("/operations/" + fullName() + "/invoke/");
        }

        virtual Value execute() {
            return crudBroker_->updateObject("/operations/" + fullName() + "/execute/");
        }

        virtual std::shared_ptr<OperationOutletAPI> outlet() const {
            
        }

        virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const = 0;
    };

    }
}