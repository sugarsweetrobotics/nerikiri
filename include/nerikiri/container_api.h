#pragma once

#include <nerikiri/object.h>
#include <nerikiri/operation_api.h>
#include <nerikiri/logger.h>
#include <nerikiri/geometry.h>

namespace nerikiri {


    class ContainerAPI : public Object {
    public:
        static constexpr char classNameString[] = "Container"; 
        
    public:
        std::shared_ptr<OperationAPI> getPoseOperation_;
        std::shared_ptr<OperationAPI> setPoseOperation_;
    public:
        std::shared_ptr<OperationAPI> getPoseOperation() const { return getPoseOperation(); }
        std::shared_ptr<OperationAPI> setPoseOperation() const { return setPoseOperation(); }

    public:
        ContainerAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}

        virtual ~ContainerAPI() {}


        virtual TimedPose3D getPose() const = 0;
        virtual void setPose(const TimedPose3D& pose) = 0;
        virtual void setPose(TimedPose3D&& pose) = 0;

        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const = 0;

        virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const = 0;

        virtual Value addOperation(const std::shared_ptr<OperationAPI>& operation) = 0;

        virtual Value deleteOperation(const std::string& fullName) = 0;
        
    };

    std::shared_ptr<ContainerAPI> nullContainer();

    template<>
    inline std::shared_ptr<ContainerAPI> nullObject() { return nullContainer(); }


    using ContainerFactoryAPI = FactoryBase<ContainerAPI>;
    using NullContainerFactory = NullFactory<ContainerAPI>;

    template<>
    inline std::shared_ptr<ContainerFactoryAPI> nullObject() { return std::make_shared<NullContainerFactory>(); }
}