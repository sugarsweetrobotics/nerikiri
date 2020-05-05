#include "nerikiri/util/logger.h"

#include "nerikiri/ec.h"
using namespace nerikiri;


std::shared_ptr<ExecutionContext> ExecutionContext::null(new ExecutionContext());