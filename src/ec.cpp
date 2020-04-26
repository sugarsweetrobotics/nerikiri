#include "nerikiri/ec.h"
#include "nerikiri/logger.h"

using namespace nerikiri;


std::shared_ptr<ExecutionContext> ExecutionContext::null(new ExecutionContext());