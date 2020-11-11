#include <nerikiri/operation_api.h>
#include <nerikiri/operation_outlet_api.h>
#include <nerikiri/operation_inlet_api.h>
using namespace nerikiri;

std::shared_ptr<OperationOutletAPI> NullOperation::outlet() const { 
    logger::error("NullOperation::{}() failed. Operation is null.", __func__);
    return std::make_shared<NullOperationOutlet>();
}

std::vector<std::shared_ptr<OperationInletAPI>> NullOperation::inlets() const {
    logger::error("NullOperation::{}() failed. Operation is null.", __func__);
    return {};
}