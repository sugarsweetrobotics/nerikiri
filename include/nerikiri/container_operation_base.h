#pragma once

#include <mutex>
#include <thread>

#include <nerikiri/operation.h>
#include <nerikiri/container.h>

namespace nerikiri {
    class ContainerOperationBase : public OperationAPI {
    protected:
        std::shared_ptr<OperationAPI> base_;
        std::shared_ptr<ContainerAPI> container_;
        std::mutex mutex_;
    public:
        ContainerOperationBase(const std::shared_ptr<ContainerAPI>& container, const std::string& _typeName, const std::string& _fullName, const Value& defaultArgs)
          : OperationAPI("ContainerOperation", nerikiri::naming::join(container->fullName(), _typeName), _fullName),
          base_(createOperation(nerikiri::naming::join(container->fullName(), _typeName), _fullName, defaultArgs, [this](auto value) {
              return this->call(value);
          })), container_(container){
              
          }

        virtual ~ContainerOperationBase() {}
        virtual Value fullInfo() const override { 
            auto i = base_->fullInfo(); 
            i["ownerContainerFullName"] = container_->fullName();
            return i;
        }


        /**
         * This function collects the value from inlets and calls the function inside the Operation,
         * but this does not execute the output side operation
         */
        virtual Value invoke() override { return base_->invoke(); }

        virtual Value execute() override { return base_->execute(); }

        virtual std::shared_ptr<OperationOutletAPI> outlet() const override { return base_->outlet(); }

        virtual std::shared_ptr<OperationInletAPI> inlet(const std::string& name) const override { return base_->inlet(name); }
    
        virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override { return base_->inlets(); }

        
    };

    

}
