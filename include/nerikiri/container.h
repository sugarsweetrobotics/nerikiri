#pragma once

#include "nerikiri/value.h"
#include "nerikiri/object.h"
#include "nerikiri/naming.h"

#include "nerikiri/containerbase.h"
#include "nerikiri/operationbase.h"
#include "nerikiri/containeroperationfactorybase.h"

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
        Container(ContainerFactoryBase* parentFactory, const Value& info): ContainerBase(parentFactory, demangle(typeid(T).name()), info), _ptr(std::make_shared<T>()) {
            if (!containerNameValidator(info.at("typeName").stringValue())) {
                //is_null_ = true;
                _setNull();
            }
        }
        virtual ~Container() {}
 
        std::shared_ptr<T>& ptr() { return _ptr; }

        std::shared_ptr<T>& operator->() { return ptr(); }
    };

            

}