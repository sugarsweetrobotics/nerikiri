#pragma once

#include <mutex>
#include <thread>

#include <nerikiri/operation_api.h>
#include <nerikiri/container_api.h>
#include <nerikiri/operation.h>
//#include <nerikiri/container.h>

namespace nerikiri {


    class ContainerOperationBase : public OperationAPI {
    public:
        static constexpr char classNameString[] = "ContainerOperation"; 
    protected:
        std::shared_ptr<OperationAPI> base_;
        std::shared_ptr<ContainerAPI> container_;
        std::mutex mutex_;
        Value defaultArgs_;
    public:


        virtual Value setOwner(const std::shared_ptr<Object>& container) override;


    public:
        ContainerOperationBase(const std::string& _typeName, const std::string& _fullName, const Value& defaultArgs)
          : OperationAPI("ContainerOperation", _typeName, _fullName), defaultArgs_(defaultArgs)
           {}

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
