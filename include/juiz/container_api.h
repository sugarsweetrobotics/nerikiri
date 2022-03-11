#pragma once
#include <filesystem>
#include <fstream>

#include <juiz/object.h>
#include <juiz/operation_api.h>
#include <juiz/logger.h>
#include <juiz/geometry.h>
#include <juiz/utils/yaml.h>


namespace juiz {

    using JUIZ_MESH_DATA = Value;


    class ContainerAPI : public Object {
    public:
        static constexpr char classNameString[] = "Container"; 
        
    public:
        std::shared_ptr<OperationAPI> getPoseOperation_;
        std::shared_ptr<OperationAPI> setPoseOperation_;
    public:
        std::shared_ptr<OperationAPI> getPoseOperation() const { return getPoseOperation_; }
        std::shared_ptr<OperationAPI> setPoseOperation() const { return setPoseOperation_; }

    public:
        ContainerAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}

        virtual ~ContainerAPI() {}

        virtual TimedPose3D getPose() const = 0;
        
        virtual void setPose(const TimedPose3D& pose) = 0;

        virtual void setPose(TimedPose3D&& pose) = 0;

        virtual JUIZ_MESH_DATA getMeshData() const = 0;

        virtual void setMeshData(const JUIZ_MESH_DATA& mesh) = 0;

        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const = 0;

        virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const = 0;

        virtual Value addOperation(const std::shared_ptr<OperationAPI>& operation) = 0;

        virtual Value deleteOperation(const std::string& fullName) = 0;
        
    };

    std::shared_ptr<ContainerAPI> nullContainer();

    template<>
    inline std::shared_ptr<ContainerAPI> nullObject() { return nullContainer(); }


    



    class ContainerFactoryAPI : public FactoryAPI {
    public:

        ContainerFactoryAPI(const std::string& typeName, const std::string& fullName): FactoryAPI("ContainerFactory", typeName, fullName) {}

        virtual ~ContainerFactoryAPI() {}

        virtual void setMeshData(const JUIZ_MESH_DATA& mesh) = 0;

        virtual JUIZ_MESH_DATA getMeshData() const = 0;

        static Value loadMesh(const Value& value) {
            if (value.hasKey("include")) {
                std::filesystem::path path(value["include"].stringValue());
                if (path.extension() == ".yaml" || path.extension() == ".yml") {
                    std::ifstream src_f(path);
                    return juiz::yaml::toValue(src_f);
                }
            }
            return value;
        }

    };

    class NullContainerFactory : public ContainerFactoryAPI {
    public:

        NullContainerFactory(): ContainerFactoryAPI("NullContainer", "null") {}

        virtual ~NullContainerFactory() {}

        virtual void setMeshData(const JUIZ_MESH_DATA& mesh) override {
            logger::error("NullContainerFactory::{}() failed. ContainerFactory is null.", __func__);
        }

        virtual JUIZ_MESH_DATA getMeshData() const override {
            return Value::error(logger::error("NullContainerFactory::{}() failed. ContainerFactory is null.", __func__));            
        }


        virtual std::shared_ptr<Object> create(const std::string& _fullName, const Value& info={}) const override {
            logger::error("NullContainerFactory::{}() failed. ContainerFactory is null.", __func__);
            return nullObject<ContainerAPI>();  
        }
    };

    template<>
    inline std::shared_ptr<ContainerFactoryAPI> nullObject() { return std::make_shared<NullContainerFactory>(); }



    class ContainerOperationFactoryAPI : public FactoryAPI {
    public:
        ContainerOperationFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName)
        : FactoryAPI(className, typeName, fullName) {}
        virtual ~ContainerOperationFactoryAPI() {}
        
        // virtual std::shared_ptr<Object> create(const std::string& _fullName, const Value& info=Value::error("")) const = 0;
    };

    std::shared_ptr<ContainerOperationFactoryAPI> nullContainerOperationFactory();

    template<>
    inline std::shared_ptr<ContainerOperationFactoryAPI> nullObject() { return nullContainerOperationFactory(); }
}