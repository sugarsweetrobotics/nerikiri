#pragma once

#include <string>
#include "nerikiri/value.h"

namespace nerikiri::json {

    class JSONParseError : public std::exception {
    public:
        const char* what() throw() { return "JSONParseError"; }
    };

    std::string toJSONString(const nerikiri::Value& value);

    nerikiri::Value toValue(const std::string& json_str);

};