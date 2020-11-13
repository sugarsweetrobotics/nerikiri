#include <nerikiri/object.h>


using namespace nerikiri;

Object::Object(const std::string& className, const std::string& typeName, const std::string& fullName) {
    auto [nameSpace, instanceName] = separateNamespaceAndInstanceName(fullName);
    info_ = Value{
        {"className", className},
        {"typeName", typeName},
        {"instanceName", instanceName},
        {"fullName", fullName},
        {"state", "created"}
    };
}



Object::Object(const std::string& typeName, const std::string& fullName) {
    auto [nameSpace, instanceName] = separateNamespaceAndInstanceName(fullName);
    info_ = Value{
        {"typeName", typeName},
        {"instanceName", instanceName},
        {"fullName", fullName},
        {"state", "created"}
    };
}

Object::Object(const Value& info) : info_(info){
    info_["state"] = "created";
    if (!info_.hasKey("typeName")) {
    nerikiri::logger::error("Object::Object(const Value& v={}): Error. No typeName member is included in the argument 'info'", info);
    } else if (!info_.hasKey("instanceName")) {
    if (info_.hasKey("fullName")) {
        nerikiri::logger::warn("Object::Object(const Value& v={}): Warning. No instanceName member is included in the argument 'info', but info has fullName member, so the instanceName of this object is now '{}'", info, info.at("fullName"));
        info_["instanceName"] = info_["fullName"];
    } else {
        nerikiri::logger::error("Object::Object(const Value& v={}): Error. No instanceName member is included in the argument 'info'", info);
    }
    } else if (!info_.hasKey("fullName")) {
    if (info_.hasKey("instanceName")) {
        nerikiri::logger::warn("Object::Object(const Value& v={}): Warning. No fullName member is included in the argument 'info', but info has instanceName member, so the fullName of this object is now '{}'", info, info.at("instanceName"));
        info_["fullName"] = info_["instanceName"];
    } else {
        nerikiri::logger::error("Object::Object(const Value& v={}): Error. No fullName member is included in the argument 'info'", info);
    }
    }
}

Object::Object(): Object({{"typeName", "null"}, {"instanceName", "null"}, {"fullName", "null"}, {"state", "created"}}) {}


Object::~Object() {}

Value Object::info() const {
    return {
        {"fullName", fullName()},
        {"instanceName", instanceName()},
        {"typeName", typeName()},
        {"state", info_.at("state")}
    }; 
}


std::string Object::fullName() const { 
    if (info_.objectValue().count("fullName") == 0) { return ""; }
    else if (!info_.at("fullName").isStringValue()) { 
    return ""; 
    }
    return info_.at("fullName").stringValue();
}

std::string Object::getFullName() const { return fullName(); }

Value Object::setFullName(const std::string& nameSpace, const std::string& name) {
    info_["instanceName"] = name;
    if (nameSpace.length() == 0) info_["fullName"] = name;
    else info_["fullName"] = nameSpace + ":" + name;
    return info_;
}

Value Object::setFullName(const std::string& fullName) {
    auto [nameSpace, instanceName] = separateNamespaceAndInstanceName(fullName);
    info_["instanceName"] = instanceName;
    info_["fullName"] = fullName;
    return info_;
}  

std::string Object::instanceName() const { 
      return info_.at("instanceName").stringValue();
    }

    std::string Object::getInstanceName() const { 
      return instanceName();
    }