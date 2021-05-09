#pragma once

#include <nerikiri/container_api.h>
#include <nerikiri/geometry.h>

namespace nerikiri {


    // class ContainerOperationBase;
    class ContainerOperationFactoryBase;
    class ContainerFactoryBase;

    //class OperationBase;
    std::shared_ptr<ContainerAPI> containerBase(const ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName);
    std::shared_ptr<OperationAPI> containerOperationBase(const std::string& _typeName, const std::string& _fullName, const Value& defaultArgs, const std::function<Value(const Value&)> func);

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
        
        virtual void setTypeName(const std::string& typeName) override { base_->setTypeName(typeName); }
        virtual void setClassName(const std::string& className) override { base_->setClassName(className); }
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

/*
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
*/

    template<typename T>
    class ContainerOperation : public OperationAPI {
    private:
        static constexpr char classNameString[] = "ContainerOperation"; 
    protected:
        std::function<Value(T&,const Value&)> function_;
        std::shared_ptr<OperationAPI> base_;
        std::mutex mutex_;
        Value defaultArgs_;
    public:
        virtual Value setOwner(const std::shared_ptr<Object>& container) override { return base_->setOwner(container); }
        virtual const std::shared_ptr<Object> getOwner() const override { return base_->getOwner(); }

    public:
        ContainerOperation(const std::string& _typeName, const std::string& _fullName, const Value& defaultArgs, const std::function<Value(T&,const Value&)>& func)
         : OperationAPI("ContainerOperation", _typeName, _fullName), 
            base_(containerOperationBase(_typeName, _fullName, defaultArgs, [this](const Value& v) {return this->call(v); })),
            defaultArgs_(defaultArgs),
            function_(func)
           {}

        virtual ~ContainerOperation() {}

        virtual Value call(const Value& value) override {
            std::lock_guard<std::mutex> lock(mutex_);
            auto c = std::static_pointer_cast<Container<T>>(getOwner());
            if (!c) {
                logger::error("ContainerOperation::call failed. Invalid owner container pointer. Can not convert to {}. ", nerikiri::demangle(typeid(T).name()));
            }
            return this->function_(*(c->ptr()), value);
        }

        virtual Value info() const override {
            return base_->info();
        }

        virtual Value fullInfo() const override { 
            auto i = base_->fullInfo(); 
            i["ownerContainerFullName"] = getOwner()->fullName();
            return i;
        }

        virtual Value invoke() override { return base_->invoke(); }

        virtual Value execute() override { return base_->execute(); }

        virtual std::shared_ptr<OperationOutletAPI> outlet() const override { return base_->outlet(); }

        virtual std::shared_ptr<OperationInletAPI> inlet(const std::string& name) const override { return base_->inlet(name); }
    
        virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override { return base_->inlets(); }        
    };


    class ContainerGetPoseOperation : public OperationAPI {
    private:  
        std::mutex mutex_;
        std::shared_ptr<OperationAPI> base_;

    public:
        ContainerGetPoseOperation()
          : OperationAPI("ContainerOperation", "container_get_pose", "container_get_pose.ope"),
          base_(containerOperationBase("container_get_pose", "container_get_pose.ope", {}, [this](const Value& v) {return this->call(v); }))
             {}
        virtual ~ContainerGetPoseOperation() {}

        virtual Value setOwner(const std::shared_ptr<Object>& container) override { return base_->setOwner(container); }
        virtual const std::shared_ptr<Object> getOwner() const override { return base_->getOwner(); }

        virtual Value call(const Value& value) override  {
            std::lock_guard<std::mutex> lock(this->mutex_);

            auto c = std::static_pointer_cast<ContainerAPI>(getOwner());
            if (!c) {

            }
            auto pose = c->getPose();
            return toValue(pose);
        }


        virtual Value info() const override { return base_->info(); }

        virtual Value fullInfo() const override { 
            auto i = base_->fullInfo(); 
            i["ownerContainerFullName"] = getOwner()->fullName();
            return i;
        }

        virtual Value invoke() override { return base_->invoke(); }

        virtual Value execute() override { return base_->execute(); }

        virtual std::shared_ptr<OperationOutletAPI> outlet() const override { return base_->outlet(); }

        virtual std::shared_ptr<OperationInletAPI> inlet(const std::string& name) const override { return base_->inlet(name); }
    
        virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override { return base_->inlets(); }
    };

    class ContainerSetPoseOperation : public OperationAPI {
    private:
        std::mutex mutex_;
        Value defaultArgs_;
        std::shared_ptr<OperationAPI> base_;
    public:
        ContainerSetPoseOperation()
          : OperationAPI("ContainerOperation", "container_set_pose", "container_set_pose.ope"),
          defaultArgs_( 
                {
                  {"pose", toValue(TimedPose3D())}
                } 
          ),
          base_(containerOperationBase("container_get_pose", "container_get_pose.ope", {}, [this](const Value& v) {return this->call(v); }))
           {}
        virtual ~ContainerSetPoseOperation() {}
        
        virtual Value setOwner(const std::shared_ptr<Object>& container) override { return base_->setOwner(container); }
        virtual const std::shared_ptr<Object> getOwner() const override { return base_->getOwner(); }

        virtual Value call(const Value& value) override {
            std::lock_guard<std::mutex> lock(this->mutex_);
            auto c = std::static_pointer_cast<ContainerAPI>(getOwner());
            if (!c) {

            }

            auto pose = toTimedPose3D(value["pose"]);
            c->setPose(pose);
            return value;
        }


        virtual Value info() const override { return base_->info(); }

        virtual Value fullInfo() const override { 
            auto i = base_->fullInfo(); 
            i["ownerContainerFullName"] = getOwner()->fullName();
            return i;
        }

        virtual Value invoke() override { return base_->invoke(); }

        virtual Value execute() override { return base_->execute(); }

        virtual std::shared_ptr<OperationOutletAPI> outlet() const override { return base_->outlet(); }

        virtual std::shared_ptr<OperationInletAPI> inlet(const std::string& name) const override { return base_->inlet(name); }
    
        virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override { return base_->inlets(); }
    };


    /**
     * ContainerFactoryテンプレートクラス
     */
    template<typename T>
    class ContainerFactory : public ContainerFactoryAPI {
    private:
      const Value defaultInfo_;
    public:

        /**
         * コンストラクタ
         */
      ContainerFactory(const Value& info={}): ContainerFactoryAPI(demangle(typeid(T).name()), demangle(typeid(T).name())), defaultInfo_(info) {}


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
            logger::trace("ContainerFactory<{}>::create(fullName={}) called.", typeName(), fullName);
            auto c = std::make_shared<Container<T>>(this, fullName);
            c->getPoseOperation_ = std::make_shared<ContainerGetPoseOperation>();
            c->getPoseOperation_->setOwner(c);
            c->setPoseOperation_ = std::make_shared<ContainerSetPoseOperation>();
            c->setPoseOperation_->setOwner(c);

	    if (defaultInfo_.hasKey("className")) {
	      c->setClassName(defaultInfo_["className"].stringValue());
	    }
            return c;
        }
    };

    /**
     * ContainerFactoryの生成．ユーザはこの関数を使ってContainerFactoryを定義，アクセスできる
     */
    template<typename T>
    void* containerFactory(const Value& info={}) {
        logger::trace("nerikiri::containerFactory<{}> called.", demangle(typeid(T).name()));
        return new ContainerFactory<T>(info); 
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
        logger::trace("nerikiri::containerOperationFactory<{}> called.", demangle(typeid(T).name()));
        return new ContainerOperationFactory<T>(Value::string(info["typeName"]), info["defaultArg"], func);
    }

}
