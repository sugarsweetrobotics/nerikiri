#pragma once

#include <sstream>
#include <string>
#include <cstdio>
#include "nerikiri/nerikiri.h"
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

    std::string NK_API toJSONString(const nerikiri::Value& value);

    nerikiri::Value NK_API toValue(const std::string& json_str);

    nerikiri::Value NK_API toValue(std::FILE* fp);

};