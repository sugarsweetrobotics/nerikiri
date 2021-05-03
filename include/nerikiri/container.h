#pragma once

#include <nerikiri/container_api.h>
#include <nerikiri/geometry.h>
#include <nerikiri/container_operation_base.h>

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


    /**
     * ContainerFactoryテンプレートクラス
     */
    template<typename T>
    class ContainerFactory : public ContainerFactoryAPI {
    public:

        /**
         * コンストラクタ
         */
        ContainerFactory(): ContainerFactoryAPI(demangle(typeid(T).name()), demangle(typeid(T).name())) {}


        /**
         * 
         */
        virtual ~ContainerFactory() {}
    public:


        /**
         * 
         * 
         */
        virtual std::shared_ptr<Object> create(const std::string& fullName, const Value& info={}) const override { 
            logger::info("ContainerFactory<{}>::create(fullName={}) called.", typeName(), fullName);
            auto c = std::make_shared<Container<T>>(this, fullName);
            c->getPoseOperation_ = std::make_shared<ContainerGetPoseOperation>();
            c->getPoseOperation_->setOwner(c);
            c->setPoseOperation_ = std::make_shared<ContainerSetPoseOperation>();
            c->setPoseOperation_->setOwner(c);
            return c;
        }
    };

    /**
     * ContainerFactoryの生成．ユーザはこの関数を使ってContainerFactoryを定義，アクセスできる
     */
    template<typename T>
    void* containerFactory() {
        logger::info("nerikiri::containerFactory<{}> called.", demangle(typeid(T).name()));
        return new ContainerFactory<T>(); 
    }


    /**
     * 
     */
    template<typename T>
    class ContainerOperationFactory : public ContainerOperationFactoryAPI {
    private:
        const Value defaultArgs_;
        const std::function<Value(T&,const Value&)> function_;
    public:
        /**
         * コンストラクタ
         * @param typeName: オペレーションのtypeName
         */
        ContainerOperationFactory(const std::string& typeName, const Value& defaultArgs, std::function<Value(T&,const Value&)> func)
          : ContainerOperationFactoryAPI("ContainerOperationFactory", naming::join(nerikiri::demangle(typeid(T).name()), typeName), naming::join(nerikiri::demangle(typeid(T).name()), typeName)), defaultArgs_(defaultArgs), function_(func)
        {}

        /**
         * デストラクタ
         */
        virtual ~ContainerOperationFactory() {}
    public:

        /**
         * 
         */
        virtual std::shared_ptr<Object> create(const std::string& fullName, const Value& info=Value::error("")) const override { 
            auto defaultArg = defaultArgs_;
            if (info.isError()) {
                //defaultArg = info["defaultArg"];
            }
            if (!info.isError() && info.hasKey("defaultArg")) {
                defaultArg = Value::merge(defaultArg, info["defaultArg"]);    
            }
            return std::make_shared<ContainerOperation<T>>(typeName(), fullName, defaultArg, function_);
        }
    };

    /**
     * 
     */
    template<typename T>
    void* containerOperationFactory(const Value& info, const std::function<Value(T&,const Value&)>& func) { 
        return new ContainerOperationFactory<T>(Value::string(info["typeName"]), info["defaultArg"], func);
    }

}
