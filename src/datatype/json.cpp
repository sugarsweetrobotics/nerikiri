#include "nerikiri/datatype/json.h"
#include "rapidjson/document.h"
#include <iostream>
#include <ios>
#include <sstream>

using namespace nerikiri::json;

namespace {
    nerikiri::Value construct(rapidjson::Value& value) {
        if(value.IsInt()) {
            return nerikiri::Value(value.GetInt());
        } else if(value.IsUint()) {
            return nerikiri::Value((int)value.GetUint());
        } else if(value.IsInt64()) {
            return nerikiri::Value(value.GetInt64());
        } else if(value.IsUint64()) {
            return nerikiri::Value((int)value.GetUint64());
        } else if (value.IsFloat()) {
            return nerikiri::Value(value.GetFloat());
        } else if (value.IsDouble()) {
            return nerikiri::Value(value.GetDouble());
        } else if (value.IsString()) {
            return nerikiri::Value(value.GetString());
        } else if (value.IsArray()) {
            auto a = value.GetArray();
            std::vector<nerikiri::Value> vs;
            for(auto& e : a) {
                vs.push_back(construct(e));
            }
            return nerikiri::Value(vs);
        } 

        if (!value.IsObject()) {
            std::cerr << "JSON Parse Error. Unknown Type of Object" << std::endl;
            throw JSONParseError();
        }

        auto a = value.GetObject();
        std::vector<std::pair<std::string, nerikiri::Value>> ps;
        for(auto& e : a) {
            ps.push_back(std::pair<std::string, nerikiri::Value>(e.name.GetString(), construct(e.value)));
        }
        return nerikiri::Value(std::move(ps));

    }
}
nerikiri::Value nerikiri::json::toValue(const std::string& json_str) {
    rapidjson::Document doc;
    doc.Parse(json_str.c_str());
    if (doc.HasParseError()) {
        throw JSONParseError();
    } 
    return construct(doc);
}

std::string nerikiri::json::toJSONString(const nerikiri::Value& value) {
    if (value.isIntValue()) return std::to_string(value.intValue());
    if (value.isDoubleValue()) return std::to_string(value.doubleValue());
    if (value.isStringValue()) return std::string("\"") + value.stringValue() + "\"";
    if (value.isObjectValue()) {
        std::stringstream ss;
        for(auto [k, v] : value.objectValue()) {
            ss << ",\"" << k << "\":" << toJSONString(v);
        }
        ss << "}";
        return ss.str().replace(0, 1, "{");
    }
    if (value.isListValue()) {
        std::stringstream ss;
        for(auto& v : value.listValue()) {
            ss << "," << toJSONString(v);
        }
        ss << "]";
        return ss.str().replace(0, 1, "[");
    }
    if (value.isNull()) {
        return "{}";
    }
    return "{\"Error\": \"Value is not supported type.\"}";
}