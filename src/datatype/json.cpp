#include "nerikiri/datatype/json.h"
#include <ios>
#include <sstream>

using namespace nerikiri::json;


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
    return "{\"Error\": \"Value is not supported type.\"}";
}