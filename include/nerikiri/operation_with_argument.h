#pragma once
#include <nerikiri/operation_api.h>


namespace nerikiri {


    std::shared_ptr<OperationAPI> operationWithArgument(const std::shared_ptr<OperationAPI>& op, const Value& v);
}