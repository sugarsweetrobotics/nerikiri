#include "nerikiri/operation.h"
#include "nerikiri/logger.h"
#include "nerikiri/functional.h"

using namespace nerikiri;
using namespace nerikiri::logger;

std::shared_ptr<OperationBase> OperationBase::null = std::make_shared<OperationBase>();
