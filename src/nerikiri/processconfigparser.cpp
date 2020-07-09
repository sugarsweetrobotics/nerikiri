

#include "nerikiri/processconfigparser.h"

using namespace nerikiri;

Value ProcessConfigParser::parseProjectFile(const std::string& projectFilePath) {
    logger::trace("ProcessConfigParser::parseProjectFile({})", projectFilePath);
    auto fp = fopen(projectFilePath.c_str(), "r");
    if (fp == nullptr) {
        logger::warn("ProcessConfigParser::parseConfig failed. File pointer is NULL.");
        return Value({});
    }
    auto projectDir = projectFilePath.substr(0, projectFilePath.rfind("/")+1);
    if (projectFilePath.at(0) != '/') {
        /// 絶対パス指定ではなかった
        auto fpath = getCwd() + "/" + projectFilePath;
        projectDir = fpath.substr(0, fpath.rfind("/")+1);
    }
    logger::trace(" - projectDir = {}", projectDir);
    auto v = nerikiri::json::toValue(fp);
    v["projectDir"] = projectDir;
    v["projectFilePath"] = projectFilePath;
    auto vv = parseSubProjects(v, projectDir);
    return parseShelves(vv, projectDir);
}


Value ProcessConfigParser::parseSubProjects(const Value& value, const std::string& projectDir) {
    logger::trace("ProcessConfigParser::parseSubProjects()");
    std::map<std::string, std::string> env_dictionary;
    if (value.hasKey("includes") && value.at("includes").isListValue()) {
        Value retval({});
        value.at("includes").list_for_each([&env_dictionary, &projectDir, &retval](auto v) {
            auto projpath =  v.at("path").stringValue();
            if (projpath.find("/") != 0) {
                projpath = projectDir + projpath;
            }
            auto projDir = projpath.substr(0, projpath.rfind("/")+1);
            env_dictionary["${SubProjectDirectory}"] = projDir;
            logger::trace(" - SubProjectDirectory = {}", projDir);
            retval.operator=(merge(retval, replaceAndCopy(parseSubProject(projpath, projectDir), env_dictionary)));
        });
        return merge(retval, value);
    }
    return value;
}

Value ProcessConfigParser::parseSubProject(const std::string& projFilePath, const std::string& projectDir) {
    logger::trace("ProcessConfigParser::parseSubProject({})", projFilePath);
    auto fp = fopen(projFilePath.c_str(), "r");
    if (fp == nullptr) {
        logger::warn("ProcessConfigParser::parseSubProject failed. File is " + projFilePath);
        return Value({});
    }
    auto v = nerikiri::json::toValue(fp);
    return parseSubProjects(v, projectDir);
}


Value ProcessConfigParser::parseShelves(const Value& value, const std::string& projectDir) {
    logger::trace("ProcessConfigParser::parseShelves()");
    std::map<std::string, std::string> env_dictionary;
    if (value.hasKey("shelves") && value.at("shelves").isListValue()) {
    Value retval({});
    value.at("shelves").list_for_each([&env_dictionary, &projectDir, &retval](auto v) {
        auto shelfpath = v.at("path").stringValue();
        if (shelfpath.find("/") != 0) {
            shelfpath = projectDir + shelfpath;
        }
        auto shelfDir = shelfpath.substr(0, shelfpath.rfind("/")+1);
        env_dictionary["${ShelfDirectory}"] = shelfDir;
        logger::trace(" - ShelfDirectory = {}", shelfDir);
        retval.operator=(merge(retval, replaceAndCopy(parseShelf(shelfpath, projectDir), env_dictionary)));
    });
    return merge(retval, value);
    }
    return value;
}
 
Value ProcessConfigParser::parseShelf(const std::string& shelfFilePath, const std::string& projectDir) {
    logger::trace("ProcessConfigParser::parseShelf({})", shelfFilePath);
    auto fp = fopen(shelfFilePath.c_str(), "r");
    if (fp == nullptr) {
        logger::warn("ProcessConfigParser::parseShelf failed. File is " + shelfFilePath);
        return Value({});
    }
    auto v = nerikiri::json::toValue(fp);
    return parseShelves(v, projectDir);
}

Value ProcessConfigParser::parseConfig(std::FILE* fp, const std::string& filepath, const std::string& shelfPath/*=""*/) {
    if (fp == nullptr) {
        logger::warn("ProcessConfigParser::parseConfig failed. File pointer is NULL.");
        return Value({});
    }
    auto projectDir = filepath.substr(0, filepath.rfind("/")+1);
    auto v = nerikiri::json::toValue(fp);
    v["projectDir"] = 
    v["projectFilePath"] = filepath;
    auto conf = parseShelves(v, projectDir);
    v["projectFilePath"] = filepath;
    return conf;
}

Value ProcessConfigParser::parseConfig(const std::string& jsonStr) {
    return (nerikiri::json::toValue(jsonStr));
}