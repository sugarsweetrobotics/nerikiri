#pragma once

#include <string>
#include "nerikiri/value.h"

namespace nerikiri::json {

    std::string toJSONString(const nerikiri::Value& value);
};