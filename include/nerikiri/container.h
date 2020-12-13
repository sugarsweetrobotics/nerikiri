#pragma once

//#include <nerikiri/container_base.h>
//#include <nerikiri/operation/operation_base.h>
#include <nerikiri/container_factory_api.h>

#include <nerikiri/container_operation_factory_api.h>

namespace nerikiri {

    class ContainerOperationBase;
    class ContainerOperationFactoryBase;
    class ContainerFactoryBase;

    //class OperationBase;
    std::shared_ptr<ContainerAPI> containerBase(ContainerFactoryAPI* parentFactory, const std::string& className, const std::string& typeName, const std::string& fullName);

    /**
     * Containerテンプレートクラス
     */
    template<typename T>
    class Container : public ContainerAPI {
    private:
        std::shared_ptr<T> _ptr;
        std::shared_ptr<ContainerAPI> base_;
    public:
        Container(ContainerFactoryAPI* parentFactory, const std::string& fullName) : ContainerAPI("Container", demangle(typeid(T).name()), fullName), base_(containerBase(parentFactory, "Container", demangle(typeid(T).name()), fullName))
          ,_ptr(std::make_shared<T>()) {}
        virtual ~Container() {}


        //virtual std::string typeName() const { return base_->typeName()};
        //virtual std::string fullName() const { return base_->fullName()};
        virtual Value info() const override { return base_->info(); } 

        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override { return base_->operations(); }

        virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const override { return base_->operation(fullName); }

        virtual Value addOperation(const std::shared_ptr<OperationAPI>& operation) override { return base_->addOperation(operation); }

        virtual Value deleteOperation(const std::string& fullName) override { return base_->deleteOperation(fullName); }

        std::shared_ptr<T>& ptr() { return _ptr; }

        std::shared_ptr<T>& operator->() { return ptr(); }
    };



}