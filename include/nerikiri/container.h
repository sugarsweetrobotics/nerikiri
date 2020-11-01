#pragma once

#include <nerikiri/container_base.h>
#include <nerikiri/operation_base.h>
#include <nerikiri/container_operation_factory_base.h>

namespace nerikiri {

    class ContainerOperationBase;
    class ContainerOperationFactoryBase;
    class ContainerFactoryBase;

    class OperationBase;

    /**
     * Containerテンプレートクラス
     */
    template<typename T>
    class Container : public ContainerBase {
    private:
        std::shared_ptr<T> _ptr;

    public:
        Container(ContainerFactoryBase* parentFactory, const std::string& fullName): 
          ContainerBase(parentFactory, "Container", demangle(typeid(T).name()), fullName), _ptr(std::make_shared<T>()) {}
        virtual ~Container() {}
 
        std::shared_ptr<T>& ptr() { return _ptr; }

        std::shared_ptr<T>& operator->() { return ptr(); }
    };

            

}