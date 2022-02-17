
#include <regex>

#include <juiz/utils/yaml.h>
#include <yaml-cpp/yaml.h>
#include <juiz/logger.h>
using namespace juiz;


namespace {

   

    bool IsTrue(YAML::Node& value) {
        const auto v = value.as<std::string>();
        return (v == "y" || v == "Y" || v == "yes" || v == "Yes" || v == "YES" || v == "true"  || v == "True"  || v == "TRUE");
    }

    bool IsFalse(YAML::Node& value) {
        const auto v = value.as<std::string>();
        return (v == "n" || v == "N" || v == "no"  || v == "No"  || v == "NO"  || v == "false" || v == "False" || v == "FALSE");
    }

    bool IsBool(YAML::Node& value) {
        return IsTrue(value) || IsFalse(value);
    }

    bool GetBool(YAML::Node& value) {
        return IsTrue(value);
    }


    bool IsIntBase10(const std::string& s) {
        std::regex re("[-+]?(0|[1-9][0-9_]*)");
        std::smatch match;
        return regex_match(s, match, re);
    }

    bool IsIntBase2(const std::string& s) {
        std::regex re("[-+]?0b[0-1_]+");
        std::smatch match;
        return regex_match(s, match, re);
    }

    bool IsIntBase8(const std::string& s) {
        std::regex re("[-+]?0[0-7_]+");
        std::smatch match;
        return regex_match(s, match, re);
    }

    bool IsIntBase16(const std::string& s) {
        std::regex re("[-+]?0x[0-9a-fA-F_]+");
        std::smatch match;
        return regex_match(s, match, re);
    }

    bool IsIntBase60(const std::string& s) {
        std::regex re("[-+]?[1-9][0-9_]*(:[0-5]?[0-9])+");
        std::smatch match;
        return regex_match(s, match, re);
    }

    std::string skip_under_score(const std::string& v) {
        if (v.find('_') != std::string::npos) {
            return stringJoin(stringSplit(v, '_'));
        }
        return v;
    }

    int64_t GetInteger(YAML::Node& value) {
        const auto v = value.as<std::string>();
        if (IsIntBase10(v)) { // base 2
            
            return std::stoi(skip_under_score(v), 0, 10);
        }
        if (IsIntBase2(v)) { // base 2
            return std::stoi(skip_under_score(v.substr(2)), 0, 2);
        }
        if (IsIntBase8(v)) { // base 8
            return std::stoi(skip_under_score(v.substr(1)), 0, 8);
        }
        if (IsIntBase16(v)) { // base 16
            return std::stoi(skip_under_score(v.substr(2)), 0, 16);
        }
        if (IsIntBase60(v)) { // base 60
            return std::stoi(skip_under_score(v.substr(2)), 0, 60);
        }
        return std::stoi(skip_under_score(v), 0, 10);
    }


    bool IsInteger(YAML::Node& value) {
        const auto v = value.as<std::string>();
        return IsIntBase2(v) || IsIntBase10(v) || IsIntBase8(v) || IsIntBase16(v) || IsIntBase60(v);
    }

    bool IsIPAddress(const std::string& s) {
        std::regex re("^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])$");
        std::smatch match;
        return regex_match(s, match, re);
    }


    bool IsFloatBase10(const std::string& s) {
        std::regex re("[-+]?([0-9][0-9_]*)?\\.[0-9.]*([eE][-+][0-9]+)?");
        std::smatch match;
        return regex_match(s, match, re) && (!IsIPAddress(s));
    }

    bool IsInfinity(const std::string& s) {
        std::regex re("[-+]?\\.(inf|Inf|INF)");
        std::smatch match;
        return regex_match(s, match, re);
    }

    bool IsNaN(const std::string& s) {
        std::regex re("\\.(nan|NaN|NAN)");
        std::smatch match;
        return regex_match(s, match, re);
    }

    double GetFloat(YAML::Node& value) {
        const auto v = value.as<std::string>();
        if (IsFloatBase10(v)) { // base 10
            return std::stof(v);
        }
        if (IsInfinity(v)) {
            return std::numeric_limits<double>::infinity();
        }
        return std::numeric_limits<double>::quiet_NaN();
    }
    

    bool IsFloat(YAML::Node& value) {
        const auto v = value.as<std::string>();
        return IsFloatBase10(v) || IsInfinity(v) || IsNaN(v);
    }

    std::string GetString(YAML::Node& value) {
        return value.as<std::string>();
    }

    juiz::Value construct(YAML::Node& value) {
        if(value.IsScalar()) {
            if (IsBool(value)) {
                return juiz::Value(GetBool(value));
            }
            if (IsInteger(value)) {
                return juiz::Value(GetInteger(value));
            }
            if (IsFloat(value)) {
                return juiz::Value(GetFloat(value));
            }
            return juiz::Value(GetString(value));
        }

        if (value.IsMap()) {
            auto v = Value::object();
            for(YAML::iterator it=value.begin();it!=value.end();++it) {
                v[(it->first).as<std::string>()] = construct(it->second);
            }
            return v;
        }

        if (value.IsSequence()) {
            auto v = Value::list();
            for(YAML::iterator it=value.begin();it!=value.end();++it) {
                YAML::Node node = *it;
                v.push_back(construct(node));
            }
            return v;
        }

        return Value();
        
        /* else if(value.IsInt()) {
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
        } 
        
        
        if (value.IsArray()) {
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
        */
    }
}

juiz::Value juiz::yaml::toValue(const std::string& yaml_str) {
    YAML::Node node = YAML::Load(yaml_str);
    if (!node) {
        logger::error("YAMLParseError - String=\"{}\"", yaml_str);
        throw YAMLParseError();
    } 
    return construct(node);
}

juiz::Value juiz::yaml::toValue(std::ifstream&& ifs) {
    if (!ifs) { return Value::error(logger::error("juiz::yaml::toValue(ifstream&) failed. Input file stream is wrong. The file must NOT OPENED.")); }
    YAML::Node node = YAML::Load(ifs);
    if (!node) {
        logger::error("YAMLParseError - filestream");
        throw YAMLParseError();
    } 
    return construct(node);
}

std::string juiz::yaml::toYAMLString(const juiz::Value& value) {
    return "";
}