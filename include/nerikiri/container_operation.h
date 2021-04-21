#pragma once
#include <nerikiri/geometry.h>

#include <nerikiri/container_operation_base.h>
namespace nerikiri {

    inline TimedPose3D toTimedPose3D(const Value& value) {
        return TimedPose3D{
                    Time{Value::intValue(value["tm"]["sec"]), Value::intValue(value["tm"]["nsec"])},
                    Pose3D{
                        Point3D{
                            value["pose"]["position"]["x"].doubleValue(),
                            value["pose"]["position"]["y"].doubleValue(),
                            value["pose"]["position"]["z"].doubleValue()
                        },
                        Orientation3D{
                            value["pose"]["orientation"]["x"].doubleValue(),
                            value["pose"]["orientation"]["y"].doubleValue(),
                            value["pose"]["orientation"]["z"].doubleValue(),
                            value["pose"]["orientation"]["w"].doubleValue()
                        }
                    }
                };
    }

    inline Value toValue(const TimedPose3D& pose) {
        return {
                {"tm", {
                    {"sec", (int64_t)pose.tm.sec},
                    {"nsec", (int64_t)pose.tm.nsec}
                }},
                {"pose", {
                    {"position", {
                        {"x", pose.pose.position.x},
                        {"y", pose.pose.position.y},
                        {"z", pose.pose.position.z}
                    }},
                    {"orientation", {
                        {"x", pose.pose.orientation.x},
                        {"y", pose.pose.orientation.y},
                        {"z", pose.pose.orientation.z},
                        {"w", pose.pose.orientation.w}
                    }}
                }}
            };
    }
    
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
