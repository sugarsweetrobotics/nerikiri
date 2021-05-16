#include <juiz/object.h>


using namespace juiz;

Object::Object(const std::string& className, const std::string& typeName, const std::string& fullName):
 fullName_(fullName), className_(className), typeName_(typeName)
 {
    auto [nameSpace, instanceName] = separateNamespaceAndInstanceName(fullName);
    instanceName_ = instanceName;
    info_ = Value{
        {"className", className},
        {"typeName", typeName},
        {"instanceName", instanceName},
        {"fullName", fullName},
        {"state", "created"}
    };
}



Object::Object(const std::string& typeName, const std::string& fullName):  
fullName_(fullName), className_(""), typeName_(typeName)
 {
    auto [nameSpace, instanceName] = separateNamespaceAndInstanceName(fullName);
    instanceName_ = instanceName;
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
        juiz::logger::error("Object::Object(const Value& v={}): Error. No typeName member is included in the argument 'info'", info);
    } else if (!info_.hasKey("instanceName")) {
    if (info_.hasKey("fullName")) {
        juiz::logger::warn("Object::Object(const Value& v={}): Warning. No instanceName member is included in the argument 'info', but info has fullName member, so the instanceName of this object is now '{}'", info, info.at("fullName"));
        info_["instanceName"] = info_["fullName"];
    } else {
        juiz::logger::error("Object::Object(const Value& v={}): Error. No instanceName member is included in the argument 'info'", info);
    }
    } else if (!info_.hasKey("fullName")) {
    if (info_.hasKey("instanceName")) {
        juiz::logger::warn("Object::Object(const Value& v={}): Warning. No fullName member is included in the argument 'info', but info has instanceName member, so the fullName of this object is now '{}'", info, info.at("instanceName"));
        info_["fullName"] = info_["instanceName"];
    } else {
        juiz::logger::error("Object::Object(const Value& v={}): Error. No fullName member is included in the argument 'info'", info);
    }
    }
    fullName_ = Value::string(info_["fullName"]);
    typeName_ = Value::string(info_["typeName"]);
    className_ = Value::string(info_["className"]);
    instanceName_ = Value::string(info_["instanceName"]);
}

Object::Object(): Object({{"typeName", "null"}, {"instanceName", "null"}, {"fullName", "null"}, {"state", "created"}}) {}


Object::~Object() {}

Value Object::info() const {
    return {
        {"fullName", fullName()},
        {"instanceName", instanceName()},
        {"typeName", typeName()},
        {"className", className()},
        {"state", info_.at("state")},
        {"description", description_}
    }; 
}


std::string Object::fullName() const { 
    /*
    if (info_.objectValue().count("fullName") == 0) { return ""; }
    else if (!info_.at("fullName").isStringValue()) { 
    return ""; 
    }
    return info_.at("fullName").stringValue();
    */


    return fullName_;
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
    if (instanceName.length() == 0) {
        instanceName = nameSpace;
        nameSpace = "";
    }
    info_["instanceName"] = instanceName;
    info_["fullName"] = fullName;
    return info_;
}  

std::string Object::instanceName() const { 
    /*
    if (info_.hasKey("instanceName")) {
        return info_.at("instanceName").stringValue();
    } 
    logger::error("Object({})::instanceName() object do not have instanceName property. this is wrong case.", info_);
    return info_.at("fullName").stringValue();
    */
   return instanceName_;
}

std::string Object::getInstanceName() const { 
    return instanceName();
}