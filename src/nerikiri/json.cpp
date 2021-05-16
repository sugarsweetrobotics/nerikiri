#include <iostream>
#include <ios>
#include <sstream>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include <juiz/utils/json.h>
#include "./base64.h"
#include <juiz/logger.h>

using namespace juiz::json;

namespace {
    juiz::Value construct(rapidjson::Value& value) {
        if(value.IsBool()) {
            return juiz::Value(value.GetBool());
        } else if(value.IsInt()) {
            return juiz::Value(value.GetInt());
        } else if(value.IsUint()) {
            return juiz::Value((int)value.GetUint());
        } else if(value.IsInt64()) {
            return juiz::Value(value.GetInt64());
        } else if(value.IsUint64()) {
            return juiz::Value((int)value.GetUint64());
        } else if (value.IsFloat()) {
            return juiz::Value(value.GetFloat());
        } else if (value.IsDouble()) {
            return juiz::Value(value.GetDouble());
        } else if (value.IsString()) {
            return juiz::Value(value.GetString());
        } else if (value.IsArray()) {
            auto a = value.GetArray();
            std::vector<juiz::Value> vs;
            for(auto& e : a) {
                vs.push_back(construct(e));
            }
            return juiz::Value(vs);
        } 

        if (!value.IsObject()) {
            std::cerr << "JSON Parse Error. Unknown Type of Object" << std::endl;
            throw JSONParseError();
        }

        auto a = value.GetObject();
        std::vector<std::pair<std::string, juiz::Value>> ps;
        for(auto& e : a) {
            if (strcmp(e.name.GetString(), "__ERROR__") == 0) {
                return juiz::Value::error(e.value.GetString());
            }
            if (strcmp(e.name.GetString(), "__byte64__") == 0) {
                std::cout << "Found Base64 encoded string value." << std::endl;
                size_t sz = 0;
                uint8_t* bytes = (uint8_t*)base64Decode(e.value.GetString(), &sz, BASE64_TYPE_STANDARD);
                juiz::Value v(bytes, sz);
                free(bytes);
                return v;
            }
            ps.push_back(std::pair<std::string, juiz::Value>(e.name.GetString(), construct(e.value)));
        }
        if (ps.size() == 0) { return {}; }
        return juiz::Value(std::move(ps));

    }
}
juiz::Value juiz::json::toValue(const std::string& json_str) {
    rapidjson::Document doc;
    doc.Parse(json_str.c_str());
    if (doc.HasParseError()) {
        logger::error("JSONParseError - {}", json_str);
        throw JSONParseError();
    } 
    return construct(doc);
}


juiz::Value juiz::json::toValue(std::FILE* fp) {

    rapidjson::Document doc;
    char readBuffer[65536];
    rapidjson::FileReadStream ip(fp, readBuffer, sizeof(readBuffer));
    doc.ParseStream(ip);
    if (doc.HasParseError()) {
        logger::error("JSONParseError - {}", "FILE");
        throw JSONParseError();
    } 
    return construct(doc);

}
std::string juiz::json::toJSONString(const juiz::Value& value) {
    if (value.isBoolValue()) return std::to_string(value.boolValue());
    if (value.isIntValue()) return std::to_string(value.intValue());
    if (value.isDoubleValue()) return std::to_string(value.doubleValue());
    if (value.isStringValue()) return std::string("\"") + value.stringValue() + "\"";
    if (value.isObjectValue()) {
        if (value.objectValue().size() == 0) { return "{}"; }
        
        std::stringstream ss;
        for(auto [k, v] : value.objectValue()) {
            ss << ",\"" << k << "\":" << toJSONString(v);
        }
        ss << "}";
        return ss.str().replace(0, 1, "{");
    }
    if (value.isListValue()) {
        if (value.listValue().size() == 0) { return "[]"; }

        
        std::stringstream ss;
        for(auto& v : value.listValue()) {
            ss << "," << toJSONString(v);
        }
        ss << "]";
        return ss.str().replace(0, 1, "[");
    }
    if (value.isByteArrayValue()) {
//        return "\"hogehoge\""; 
        //auto ss = "hogehoge";
        std::cout << "bytetojson:" << value.byteArraySize() << std::endl;
        auto v = ::base64Encode((const char*)(&(value.byteArrayValue()[0])), value.byteArraySize(), ::BASE64_TYPE::BASE64_TYPE_STANDARD);
        auto s = std::string("{\"__byte64__\":\"") + v + "\"}";
        free(v);
        return s;
        //return std::string("\"") + ::base64Encode(ss, 8, ::BASE64_TYPE::BASE64_TYPE_STANDARD) + "\"";
    }
    if (value.isNull()) {
        return "{}";
    }
    if (value.isError()) {

      return "{\"__ERROR__\": \"Value is error('" + value.getErrorMessage() + "').\"}"; 
  //      throw JSONConstructError(value.getErrorMessage());
    }
    return "{\"Error\": \"Value is not supported type.\"}";
}