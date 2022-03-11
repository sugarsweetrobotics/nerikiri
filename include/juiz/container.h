#pragma once

#include <queue>
#include <filesystem>
#include <thread>

#include <juiz/container_api.h>
#include <juiz/model_data.h>
#include <juiz/geometry.h>
#include <juiz/utils/yaml.h>

namespace juiz {

    const std::string _default_model_data_str = "";

    // class ContainerOperationBase;
    class ContainerOperationFactoryBase;
    class ContainerFactoryBase;

    //class OperationBase;
    std::shared_ptr<ContainerAPI> containerBase(const ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName);
    std::shared_ptr<OperationAPI> containerOperationBase(const std::string& _typeName, const std::string& _fullName, const Value& defaultArgs, const std::function<Value(const Value&)> func, const Value& info={});

    class container_task {
    public:
        std::function<void(void)> service_;
        //std::mutex mutex_;
        //std::condition_variable cv_;
        //bool notify_;
        container_task(const std::function<void(void)>& f) : service_(f) {} //, notify_(false) {}
        container_task(const container_task& t): service_(t.service_) {} //, notify_(false) {}
        //container_task(container_task&& c): service_(c.service_), notify_(false) {}
        /*
        void wait() { 
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [&]{ return notify_; });
        }

        void notify() {
            std::unique_lock<std::mutex> lock(mutex_);
            notify_ = true;
            lock.unlock();
            cv_.notify_one();
        }
        */

        void operator()() {
            service_();
        }
    };
    /**
     * Containerテンプレートクラス
     */
    template<typename T>
    class Container : public ContainerAPI {
    private:
        std::shared_ptr<T> _ptr;
        std::shared_ptr<ContainerAPI> base_;
        std::thread thread_;
        bool end_flag_;
        std::queue<container_task> task_queue_;
        bool use_thread_;
        std::condition_variable threading_condition_variable_;
        std::mutex threading_mutex_;
        bool threading_notify_;
        std::condition_variable task_condition_variable_;
        std::mutex task_mutex_;
        bool task_notify_;
        
        std::shared_ptr<ModelDataAPI> model_data_;
    public:
        void useThread(bool flag) { use_thread_ = flag; }
        bool isUseThread() const { return use_thread_; }

        virtual TimedPose3D getPose() const override { return base_->getPose(); }
        virtual void setPose(const TimedPose3D& pose) override { base_->setPose(pose); }
        virtual void setPose(TimedPose3D&& pose) override { base_->setPose(std::move(pose)); }

        virtual JUIZ_MESH_DATA getMeshData() const override { return base_->getMeshData(); }
        virtual void setMeshData(const JUIZ_MESH_DATA& mesh) override { base_->setMeshData(mesh); }
        
        virtual void setTypeName(const std::string& typeName) override { base_->setTypeName(typeName); }
        virtual void setClassName(const std::string& className) override { base_->setClassName(className); }

        virtual void setModelDataPath(const std::string& path) { model_data_ = modelDataFromPath(path); }
        virtual void setModelDataString(const std::string& str) { model_data_ = modelDataFromString(str); }
        virtual std::shared_ptr<ModelDataAPI> getModelData() const { return model_data_; }

    public:
        Container(const ContainerFactoryAPI* parentFactory, const std::string& fullName) : ContainerAPI("Container", demangle(typeid(T).name()), fullName), base_(containerBase(parentFactory, "Container", demangle(typeid(T).name()), fullName))
          ,_ptr(std::make_shared<T>()), end_flag_(false), use_thread_(false), threading_notify_(false), task_notify_(false), thread_([this]() { this->thread_routine(); }), model_data_(nullptr)
        {}
        virtual ~Container() {
            end_flag_ = true;
            threading_notify_ = true;
            threading_condition_variable_.notify_all();
            thread_.join();
        }

        void thread_routine() {
            const auto id = this->thread_.get_id();
            const auto h = std::hash<std::thread::id>{}(std::this_thread::get_id());
            while(!end_flag_) {
                std::unique_lock<std::mutex> lock(threading_mutex_);
                if (!task_queue_.empty()) {
                    {
                        std::unique_lock<std::mutex> task_lock(task_mutex_);
                        auto task = task_queue_.front();
                        logger::trace("Container::thread_routine(id={}) is executing task.", (int32_t)h);
                        task();
                        task_notify_ = true;
                    }
                    task_condition_variable_.notify_one();
                    task_queue_.pop();
                } else {
                    threading_notify_ = false;
                    threading_condition_variable_.wait(lock, [this](){ 
                        return this->threading_notify_; 
                    });
                }
            }
        }

        void push_task(const container_task& func) {
            std::unique_lock<std::mutex> task_lock(task_mutex_);
            task_notify_= false;
            task_queue_.push(func);
            {
                std::unique_lock<std::mutex> lock(threading_mutex_);
                threading_notify_ = true;
            }
            threading_condition_variable_.notify_one();

            task_condition_variable_.wait(task_lock, [this]() {
                return this->task_notify_;
            });
        }


        virtual Value info() const override { 
            auto i = base_->info(); 
            
            return i;
        } 

        virtual Value fullInfo() const override {
            auto inf = base_->fullInfo();
            /// inf["pose"] = toValue(getPose()); 
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
        std::mutex cop_mutex_;
        Value defaultArgs_;
    public:
        virtual Value setOwner(const std::shared_ptr<Object>& container) override { return base_->setOwner(container); }
        virtual const std::shared_ptr<Object> getOwner() const override { return base_->getOwner(); }

    public:
        ContainerOperation(const std::string& _typeName, const std::string& _fullName, const Value& defaultArgs, const std::function<Value(T&,const Value&)>& func, const Value& info={})
         : OperationAPI("ContainerOperation", _typeName, _fullName), 
            base_(containerOperationBase(_typeName, _fullName, defaultArgs, [this](const Value& v) {return this->call(v); }, info)),
            defaultArgs_(defaultArgs),
            function_(func)
           {
               logger::trace("ContainerOperation(typeName={}, fullName={}, defaultArgs={}, func, info={}) called", _typeName, _fullName, defaultArgs, info);
           }

        virtual ~ContainerOperation() {}

        virtual Value call(const Value& value) override {
            // std::lock_guard<std::mutex> lock(cop_mutex_);
            try {
                auto c = std::static_pointer_cast<Container<T>>(getOwner());
                if (!c) {
                    logger::error("ContainerOperation::call failed. Invalid owner container pointer. Can not convert to {}. ", juiz::demangle(typeid(T).name()));
                }
                if (c->isUseThread()) {
                    Value v;
                    c->push_task(container_task([this, &v, &c, &value]() {
                        try {
                        v = this->function_(*(c->ptr()), value);
                        } catch (std::exception& ex) {
                           logger::error("ContainerOperation(fullName={})::call() failed. Exception occurred in ContainerThread : {}", fullName(), std::string(ex.what()));
                        }
                    }));
                    return v;
                } else {
                    return this->function_(*(c->ptr()), value);
                }
            } catch (std::exception& ex) {
                return Value::error(logger::error("ContainerOperation(fullName={})::call() failed. Exception occurred {}", fullName(), std::string(ex.what())));
            }
        }

        virtual Value info() const override {
            return base_->info();
        }

        virtual Value fullInfo() const override { 
            auto i = base_->fullInfo(); 
            i["className"] = "ContainerOperation";
            i["ownerContainerFullName"] = getOwner()->fullName();
            return i;
        }

        virtual Value invoke() override { return base_->invoke(); }

        virtual Value execute() override { return base_->execute(); }

        virtual std::shared_ptr<OutletAPI> outlet() const override { return base_->outlet(); }

        virtual std::shared_ptr<InletAPI> inlet(const std::string& name) const override { return base_->inlet(name); }
    
        virtual std::vector<std::shared_ptr<InletAPI>> inlets() const override { return base_->inlets(); }        
    };


    class ContainerGetPoseOperation : public OperationAPI {
    private:  
        std::mutex mutex_;
        std::shared_ptr<OperationAPI> operation_base_;

    public:
        ContainerGetPoseOperation(const std::string& fullName)
          : OperationAPI("ContainerOperation", "container_get_pose", fullName),
          operation_base_(containerOperationBase("container_get_pose", fullName, {}, [this](const Value& v) {return this->call(v); }))
             {}
        virtual ~ContainerGetPoseOperation() {}

        virtual std::string fullName() const override { return operation_base_->fullName(); }

        virtual Value setOwner(const std::shared_ptr<Object>& container) override { return operation_base_->setOwner(container); }

        virtual const std::shared_ptr<Object> getOwner() const override { return operation_base_->getOwner(); }

        virtual Value call(const Value& value) override  {
            std::lock_guard<std::mutex> lock(this->mutex_);

            auto c = std::static_pointer_cast<ContainerAPI>(getOwner());
            if (!c) {

            }
            auto pose = c->getPose();
            return toValue(pose);
        }


        virtual Value info() const override { return operation_base_->info(); }

        virtual Value fullInfo() const override { 
            auto i = operation_base_->fullInfo(); 
            i["ownerContainerFullName"] = getOwner()->fullName();
            return i;
        }

        virtual Value invoke() override { return operation_base_->invoke(); }

        virtual Value execute() override { return operation_base_->execute(); }

        virtual std::shared_ptr<OutletAPI> outlet() const override { return operation_base_->outlet(); }

        virtual std::shared_ptr<InletAPI> inlet(const std::string& name) const override { return operation_base_->inlet(name); }
    
        virtual std::vector<std::shared_ptr<InletAPI>> inlets() const override { return operation_base_->inlets(); }
    };

    class ContainerSetPoseOperation : public OperationAPI {
    private:
        std::mutex mutex_;
        Value defaultArgs_;
        std::shared_ptr<OperationAPI> operation_base_;
    public:
        ContainerSetPoseOperation(const std::string& fullName)
          : OperationAPI("ContainerOperation", "container_set_pose", fullName),
          defaultArgs_( 
                {
                  {"pose", toValue(TimedPose3D())}
                } 
          ),
          operation_base_(containerOperationBase("container_set_pose", fullName, defaultArgs_, [this](const Value& v) {return this->call(v); }))
           {}
        virtual ~ContainerSetPoseOperation() {}
               
        virtual std::string fullName() const  override { return operation_base_->fullName(); }

        virtual Value setOwner(const std::shared_ptr<Object>& container) override { return operation_base_->setOwner(container); }
        virtual const std::shared_ptr<Object> getOwner() const override { return operation_base_->getOwner(); }

        virtual Value call(const Value& value) override {
            std::lock_guard<std::mutex> lock(this->mutex_);
            auto c = std::static_pointer_cast<ContainerAPI>(getOwner());
            if (!c) {

            }

            auto pose = toTimedPose3D(value["pose"]);
            c->setPose(pose);
            return value["pose"];
        }


        virtual Value info() const override { return operation_base_->info(); }

        virtual Value fullInfo() const override { 
            auto i = operation_base_->fullInfo(); 
            i["ownerContainerFullName"] = getOwner()->fullName();
            return i;
        }

        virtual Value invoke() override { return operation_base_->invoke(); }

        virtual Value execute() override { return operation_base_->execute(); }

        virtual std::shared_ptr<OutletAPI> outlet() const override { return operation_base_->outlet(); }

        virtual std::shared_ptr<InletAPI> inlet(const std::string& name) const override { return operation_base_->inlet(name); }
    
        virtual std::vector<std::shared_ptr<InletAPI>> inlets() const override { return operation_base_->inlets(); }
    };


    /**
     * ContainerFactoryテンプレートクラス
     */
    template<typename T>
    class ContainerFactory : public ContainerFactoryAPI {
    private:
      Value defaultInfo_;
    public:

        /**
         * コンストラクタ
         */
      ContainerFactory(const Value& info={}): ContainerFactoryAPI(demangle(typeid(T).name()), demangle(typeid(T).name())), defaultInfo_(info) {
          if (defaultInfo_.hasKey("mesh")) {
              defaultInfo_["mesh"] = loadMesh(defaultInfo_["mesh"]);
          }
      }

        

        /**
         * 
         */
        virtual ~ContainerFactory() {}
    public:

        virtual void setMeshData(const JUIZ_MESH_DATA& mesh) override {
            defaultInfo_["mesh"] = loadMesh(mesh);
        }

        virtual JUIZ_MESH_DATA getMeshData() const override {
            return defaultInfo_["mesh"];
        }

        /**
         * 
         * 
         */
        virtual std::shared_ptr<Object> create(const std::string& fullName, const Value& info={}) const override { 
            logger::trace("ContainerFactory<{}>::create(fullName={}) called.", typeName(), fullName);
            auto c = std::make_shared<Container<T>>(this, fullName);
            c->getPoseOperation_ = std::make_shared<ContainerGetPoseOperation>(juiz::naming::join(fullName, "container_get_pose.ope"));
            c->getPoseOperation_->setOwner(c);
            c->setPoseOperation_ = std::make_shared<ContainerSetPoseOperation>(juiz::naming::join(fullName, "container_set_pose.ope"));
            c->setPoseOperation_->setOwner(c);
            c->addOperation(c->getPoseOperation_);
            c->addOperation(c->setPoseOperation_);
            // ここでinfoにthreading_containerを要求するあたいがあればuseThreadをONにする
            c->useThread(true);
            if (defaultInfo_.hasKey("className")) {
                c->setClassName(defaultInfo_["className"].stringValue());
            }
            if (defaultInfo_.hasKey("mesh")) {
                c->setMeshData(loadMesh(defaultInfo_["mesh"]));
            }
            return c;
        }
    };

    /**
     * ContainerFactoryの生成．ユーザはこの関数を使ってContainerFactoryを定義，アクセスできる
     */
    template<typename T>
    void* containerFactory(const Value& info={}) {
        logger::trace("juiz::containerFactory<{}> called.", demangle(typeid(T).name()));
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
        const Value info_;
    public:
        /**
         * コンストラクタ
         * @param typeName: オペレーションのtypeName
         */
        ContainerOperationFactory(const std::string& typeName, const Value& defaultArgs, std::function<Value(T&,const Value&)> func, const Value& info)
          : ContainerOperationFactoryAPI("ContainerOperationFactory", naming::join(juiz::demangle(typeid(T).name()), typeName), naming::join(juiz::demangle(typeid(T).name()), typeName)), defaultArgs_(defaultArgs), function_(func), info_(info)
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
            logger::trace("ContainerOperationFactory(typeName={})::create(fullName={}, info={})", typeName(), fullName, info);
            auto defaultArg = defaultArgs_;
            if (info.isError()) {
                logger::error("ContainerOperationFactory(typeName={})::create(fullname={}, info={}) failed. info is error.", typeName(), fullName, info);
                //defaultArg = info["defaultArg"];
            }
            if (!info.isError() && info.hasKey("defaultArg")) {
                defaultArg = Value::merge(defaultArg, info["defaultArg"]);    
            }
            return std::make_shared<ContainerOperation<T>>(typeName(), fullName, defaultArg, function_, Value::merge(info, info_));
        }
    };

    /**
     * 
     */
    template<typename T>
    void* containerOperationFactory(const Value& info, const std::function<Value(T&,const Value&)>& func) { 
        logger::trace("juiz::containerOperationFactory<{}> called.", demangle(typeid(T).name()));
        return new ContainerOperationFactory<T>(Value::string(info["typeName"]), info["defaultArg"], func, info);
    }

}
