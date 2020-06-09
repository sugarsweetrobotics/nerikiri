#include "nerikiri/logger.h"
#include "nerikiri/functional.h"
#include "nerikiri/naming.h"

#include "nerikiri/container.h"
#include "nerikiri/containerfactory.h"
#include "nerikiri/containeroperation.h"
#include "nerikiri/containeroperationfactory.h"

using namespace nerikiri;



std::shared_ptr<ContainerBase> ContainerBase::null = std::make_shared<ContainerBase>();

