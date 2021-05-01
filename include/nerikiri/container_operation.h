#pragma once
#include <nerikiri/geometry.h>
#include <nerikiri/container_operation_base.h>

namespace nerikiri {

    template<typename T>
    class ContainerOperation : public ContainerOperationBase {
    protected:
        std::function<Value(T&,const Value&)> function_;
    public:
        ContainerOperation(const std::string& _typeName, const std::string& _fullName, const Value& defaultArgs, const std::function<Value(T&,const Value&)>& func)
        : ContainerOperationBase(_typeName, _fullName, defaultArgs), function_(func)  {
        }

        virtual ~ContainerOperation() {}

        virtual Value call(const Value& value) override {
            std::lock_guard<std::mutex> lock(mutex_);
            auto c = std::static_pointer_cast<Container<T>>(container_);
            return this->function_(*(c->ptr()), value);
        }
    };

    class ContainerGetPoseOperation : public ContainerOperationBase {
    private:
    public:
        ContainerGetPoseOperation()
          : ContainerOperationBase("container_get_pose", "container_get_pose.ope", {}) {}
        virtual ~ContainerGetPoseOperation() {}

        virtual Value call(const Value& value) {
            std::lock_guard<std::mutex> lock(this->mutex_);
            auto pose = this->container_->getPose();
            return toValue(pose);
        }
    };

    class ContainerSetPoseOperation : public ContainerOperationBase {
    public:
        ContainerSetPoseOperation()
          : ContainerOperationBase("container_set_pose", "container_set_pose.ope", 
                {
                  {"pose", toValue(TimedPose3D())}
                } 
          ) {}
        virtual ~ContainerSetPoseOperation() {}

        virtual Value call(const Value& value) {
            std::lock_guard<std::mutex> lock(this->mutex_);
            auto pose = toTimedPose3D(value["pose"]);
            this->container_->setPose(pose);
            return value;
        }
    };
}
