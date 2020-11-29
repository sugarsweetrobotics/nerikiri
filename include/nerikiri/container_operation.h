#pragma once

#include <mutex>
#include <thread>

#include <nerikiri/container.h>

namespace nerikiri {


    std::shared_ptr<OperationAPI> containerOperationBase(const std::string& _typeName, const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {});

    template<typename T>
    class ContainerOperation : public OperationAPI {
    private:
        std::shared_ptr<OperationAPI> base_;
        std::shared_ptr<ContainerAPI> container_;
        std::function<Value(T&,Value)> function_;
        std::mutex mutex_;
    public:
        ContainerOperation(const std::shared_ptr<ContainerAPI>& container, const std::string& _typeName, const std::string& _fullName, const Value& defaultArgs={}, const std::function<Value(T&,Value)>& func=nullptr): 
          OperationAPI("ContainerOperation", nerikiri::naming::join(container->fullName(), _typeName), _fullName), base_(containerOperationBase("ContainerOperation", nerikiri::naming::join(container->fullName(), _typeName), _fullName, defaultArgs)), container_(container), function_(func) {}

        virtual ~ContainerOperation() {}
        virtual Value fullInfo() const override { return base_->fullInfo(); }


        /**
         * This function collects the value from inlets and calls the function inside the Operation,
         * but this does not execute the output side operation
         */
        virtual Value invoke() override { return base_->invoke(); }

        virtual Value execute() override { return base_->execute(); }

        virtual std::shared_ptr<OperationOutletAPI> outlet() const override { return base_->outlet(); }

        virtual std::shared_ptr<OperationInletAPI> inlet(const std::string& name) const override { return base_->inlet(name); }
    
        virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override { return base_->inlets(); }

        virtual Value call(const Value& value) override {
            std::lock_guard<std::mutex> lock(mutex_);
            auto c = std::static_pointer_cast<Container<T>>(container_);
            return this->function_(*(c->ptr()), value);
        }
    };






}