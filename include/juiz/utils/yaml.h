#pragma once

#include <sstream>
#include <fstream>
#include <string>
#include <cstdio>
#include <juiz/juiz.h>
#include <juiz/value.h>

namespace juiz::yaml {

    class YAMLParseError : public std::exception {
    public:
        const char* what() throw() { return "YAMLParseError"; }
    };

    class YAMLConstructError : public std::exception {
    private:
        std::string msg_;
    public:
        YAMLConstructError(const std::string&msg) {
            std::stringstream ss;
            ss << "YAMLConstructError(msg=" << msg << ")"; 
            msg_= ss.str();
        }

        const char* what() throw() { 
            return msg_.c_str();
        }
    };

    std::string NK_API toYAMLString(const juiz::Value& value, const bool default_flow_style=false);

    juiz::Value NK_API toValue(const std::string& yaml_str);

    juiz::Value NK_API toValue(std::ifstream&& fp);

};