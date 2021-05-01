#pragma once

// #include <nerikiri/container_factory_api.h>
#include <nerikiri/container_api.h>
#include <nerikiri/container_operation_factory_api.h>
#include <nerikiri/geometry.h>

namespace nerikiri {

    class ContainerOperationBase;
    class ContainerOperationFactoryBase;
    class ContainerFactoryBase;

    //class OperationBase;
    std::shared_ptr<ContainerAPI> containerBase(const ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName);

    /**
     * Containerテンプレートクラス
     */
    template<typename T>
    class Container : public ContainerAPI {
    private:
        std::shared_ptr<T> _ptr;
        std::shared_ptr<ContainerAPI> base_;
    public:
        virtual TimedPose3D getPose() const override { return base_->getPose(); }
        virtual void setPose(const TimedPose3D& pose) override { base_->setPose(pose); }
        virtual void setPose(TimedPose3D&& pose) override { base_->setPose(std::move(pose)); }
        

    public:
        Container(const ContainerFactoryAPI* parentFactory, const std::string& fullName) : ContainerAPI("Container", demangle(typeid(T).name()), fullName), base_(containerBase(parentFactory, "Container", demangle(typeid(T).name()), fullName))
          ,_ptr(std::make_shared<T>())
        {}
        virtual ~Container() {}

        virtual Value info() const override { 
            auto i = base_->info(); 
            return i;
        } 

        virtual Value fullInfo() const override {
            auto inf = base_->fullInfo();
            inf["pose"] = toValue(getPose()); 
            return inf;
        }

        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override { return base_->operations(); }

        virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const override { return base_->operation(fullName); }

        virtual Value addOperation(const std::shared_ptr<OperationAPI>& operation) override { return base_->addOperation(operation); }

        virtual Value deleteOperation(const std::string& fullName) override { return base_->deleteOperation(fullName); }

        std::shared_ptr<T>& ptr() { return _ptr; }

        std::shared_ptr<T>& operator->() { return ptr(); }
    };



}

//#include <nerikiri/container_operation.h>
//#include <nerikiri/container_factory.h>