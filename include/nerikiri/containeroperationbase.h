#pragma once

#include "nerikiri/containerbase.h"
#include "nerikiri/operationbase.h"

namespace nerikiri {

    class ContainerBase;


    class ContainerOperationBase : public nerikiri::OperationBase {
    private:
        bool is_null_container_operation_;
        
    protected:
        ContainerBase* container_;

    public:
        ContainerOperationBase(bool is_null) : is_null_container_operation_(is_null) {}

        ContainerOperationBase(const Value& info) : is_null_container_operation_(false), OperationBase(info) {}
    
        virtual ~ContainerOperationBase() {}
    
    public:
        virtual Value getContainerOperationInfo() const = 0;

        virtual bool isNullContainerOperation() const { return is_null_container_operation_; }

        virtual void setContainer(ContainerBase* container)  { container_ = container; }
    };

}