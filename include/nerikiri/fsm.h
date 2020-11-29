#pragma once

#include <string>
#include <map>
#include <vector>

#include "nerikiri/nerikiri.h"
//#include "nerikiri/object.h"
//#include <nerikiri/operation_base.h>
//#include "nerikiri/ec.h"

//#include "nerikiri/fsm_state.h"
#include <nerikiri/fsm_api.h>
#include <nerikiri/fsm_factory_api.h>

namespace nerikiri {


    std::shared_ptr<FSMAPI> fsm(const Value& info);
    std::shared_ptr<FSMFactoryAPI> fsmFactory(const std::string& fullName);



}