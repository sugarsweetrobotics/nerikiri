#pragma once

#include <mutex>
#include <thread>

#include <nerikiri/container_operation_base.h>

namespace nerikiri {


    template<typename T>
    class ContainerOperation : public nerikiri::ContainerOperationBase {
    private:
        std::shared_ptr<Container<T>> container_;
        std::function<Value(T&,Value)> function_;
        std::mutex mutex_;
    public:
        ContainerOperation(const std::shared_ptr<Container<T>>& container, const std::string& _typeName, const std::string& _fullName, const Value& defaultArgs = {}, const std::function<Value(T&,Value)>& func=nullptr): 
          ContainerOperationBase("ContainerOperation", nerikiri::naming::join(container->fullName(), _typeName), _fullName, defaultArgs), container_(container), function_(func) {}

        virtual ~ContainerOperation() {}

        virtual Value call(const Value& value) override {
            std::lock_guard<std::mutex> lock(mutex_);
            return this->function_(*(container_->ptr()), value);
        }
    };






}