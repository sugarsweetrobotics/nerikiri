#pragma once

#include <sstream>
#include <string>
#include <cstdio>
#include "nerikiri/value.h"

namespace nerikiri::json {

    class JSONParseError : public std::exception {
    public:
        const char* what() throw() { return "JSONParseError"; }
    };

    class JSONConstructError : public std::exception {
    private:
        std::string msg;
    public:
        JSONConstructError(const std::string&msg) : msg(msg) {}
        const char* what() throw() { 
            std::stringstream ss;
            ss << "JSONConstructError(msg=" << msg << ")"; 
            return ss.str().c_str();
        }
    };

    std::string toJSONString(const nerikiri::Value& value);

    nerikiri::Value toValue(const std::string& json_str);

    nerikiri::Value toValue(std::FILE* fp);

};