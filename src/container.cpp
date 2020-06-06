#include "nerikiri/util/logger.h"
#include "nerikiri/util/functional.h"
#include "nerikiri/util/naming.h"

#include "nerikiri/containers/container.h"
#include "nerikiri/containers/containerfactory.h"
#include "nerikiri/containers/containeroperation.h"
#include "nerikiri/containers/containeroperationfactory.h"

using namespace nerikiri;



std::shared_ptr<ContainerBase> ContainerBase::null = std::make_shared<ContainerBase>();

