#pragma once

#include <mutex>
#include <thread>

#include <nerikiri/container.h>
#include <nerikiri/container_operation_base.h>

namespace nerikiri {


    template<typename T>
    class ContainerOperation : public ContainerOperationBase {
    private:
        std::shared_ptr<ContainerAPI> container_;
        std::function<Value(T&,Value)> function_;
        std::mutex mutex_;
    public:
        ContainerOperation(const std::shared_ptr<ContainerAPI>& container, const std::string& _typeName, const std::string& _fullName, const Value& defaultArgs={}, const std::function<Value(T&,Value)>& func=nullptr): 
          ContainerOperationBase("ContainerOperation", nerikiri::naming::join(container->fullName(), _typeName), _fullName, defaultArgs), container_(container), function_(func) {}

        virtual ~ContainerOperation() {}

        virtual Value call(const Value& value) override {
            std::lock_guard<std::mutex> lock(mutex_);
            auto c = std::static_pointer_cast<Container<T>>(container_);
            return this->function_(*(c->ptr()), value);
        }
    };






}