#pragma once

#include <sstream>
#include <string>
#include <cstdio>
#include <juiz/nerikiri.h>
#include <juiz/value.h>

namespace juiz::json {

    class JSONParseError : public std::exception {
    public:
        const char* what() throw() { return "JSONParseError"; }
    };

    class JSONConstructError : public std::exception {
    private:
        std::string msg_;
    public:
        JSONConstructError(const std::string&msg) {
            std::stringstream ss;
            ss << "JSONConstructError(msg=" << msg << ")"; 
            msg_= ss.str();
        }

        const char* what() throw() { 
            return msg_.c_str();
        }
    };

    std::string NK_API toJSONString(const juiz::Value& value);

    juiz::Value NK_API toValue(const std::string& json_str);

    juiz::Value NK_API toValue(std::FILE* fp);

};