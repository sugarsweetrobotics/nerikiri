#include <filesystem>

#include <juiz/utils/yaml.h>
#include "processconfigparser.h"


using namespace juiz;
namespace fs = std::filesystem;

Value ProcessConfigParser::parseProjectFile(const std::string& projectFilePath) {
    logger::info("ProcessConfigParser::parseProjectFile({}) called", projectFilePath);
    Value config;
    if (fs::path(projectFilePath).extension() == ".nkproj") {
        logger::info(" - opening NKPROJ file.");
        // 拡張子が.nkprojであればjsonを仮定
        auto fp = fopen(projectFilePath.c_str(), "r");
        if (fp == nullptr) {
            logger::warn("ProcessConfigParser::parseConfig failed. Can not open file.  File path is {}.", projectFilePath);
            return Value({});
        }
        config = juiz::json::toValue(fp);
    } else {
        logger::info(" - opening JPROJ file.");
        config = juiz::yaml::toValue(std::ifstream(projectFilePath));
    }
    
    auto projectDir = projectFilePath.substr(0, projectFilePath.rfind("/")+1);
    if (projectFilePath.at(0) != '/') {
        /// 絶対パス指定ではなかった
        auto fpath = getCwd() + "/" + projectFilePath;
        projectDir = fpath.substr(0, fpath.rfind("/")+1);
    }
    logger::trace(" - projectDir = {}", projectDir);
    std::map<std::string, std::string> env_dictionary;
    env_dictionary["${ProjectDirectory}"] = projectDir;
    
    //auto v = juiz::json::toValue(fp);

    auto v = replaceAndCopy(config, env_dictionary);
    v["projectDir"] = projectDir;
    v["projectFilePath"] = projectFilePath;
    logger::trace(" - parsing sub projects....");
    auto vv = parseSubProjects(v, projectDir);
    logger::trace(" - parsing shelves....");
    return parseShelves(vv, projectDir);
}


Value ProcessConfigParser::parseSubProjects(const Value& value, const std::string& projectDir) {
    logger::trace("ProcessConfigParser::parseSubProjects(projectDir={}) called", projectDir);
    std::map<std::string, std::string> env_dictionary;
    // includesタグにしたがってサブプロジェクトを読み込む
    if (value.hasKey("includes") && value.at("includes").isListValue()) {
        Value retval({});
        value.at("includes").const_list_for_each([&env_dictionary, &projectDir, &retval](auto v) {
            logger::trace(" - parsing includes (path={})", v["path"].stringValue());
            auto projpath =  v.at("path").stringValue();
            if (projpath.find("/") != 0) {
                projpath = projectDir + projpath;
            }
            auto projDir = projpath.substr(0, projpath.rfind("/")+1);
            env_dictionary["${SubProjectDirectory}"] = projDir;
            env_dictionary["${ProjectDirectory}"] = projDir;
            logger::trace(" - SubProjectDirectory = {}", projDir);
            retval.operator=(merge(retval, replaceAndCopy(parseSubProject(projpath, projectDir), env_dictionary)));
        });
        return merge(retval, value);
    }
    logger::trace("ProcessConfigParser::parseSubProjects(projectDir={}) exit", projectDir);
    return value;
}

Value ProcessConfigParser::parseSubProject(const std::string& projFilePath, const std::string& projectDir) {
    logger::trace("ProcessConfigParser::parseSubProject(projFilePath={})", projFilePath);
    std::map<std::string, std::string> env_dictionary;
    env_dictionary["${ProjectDirectory}"] = projectDir;
    std::filesystem::path _path(projFilePath);
    if (_path.extension() == ".nkproj") {
        auto fp = fopen(projFilePath.c_str(), "r");
        if (fp == nullptr) {
            logger::warn("ProcessConfigParser::parseSubProject failed. Can not open file. File path is " + projFilePath);
            logger::trace("ProcessConfigParser::parseSubProject(projFilePath={}) exit", projFilePath);
            return Value({});
        }
        auto v = replaceAndCopy(juiz::json::toValue(fp), env_dictionary);
        logger::trace("ProcessConfigParser::parseSubProject(projFilePath={}) exit", projFilePath);
        return parseSubProjects(v, projectDir);
    } else if (_path.extension() == ".jproj") {
        auto v = replaceAndCopy(juiz::yaml::toValue(std::ifstream(_path)), env_dictionary);
        logger::trace("ProcessConfigParser::parseSubProject(projFilePath={}) exit", projFilePath);
        return parseSubProjects(v, projectDir);
    }
    return Value::error(logger::error("ProcessConfigParser::parseSubProject({}) failed. Invalid File extension. Must be .nkproj or .jproj", projFilePath));
}


Value ProcessConfigParser::parseShelves(const Value& value, const std::string& projectDir) {
    logger::trace("ProcessConfigParser::parseShelves(projectDir={}) called", projectDir);
    if (value.hasKey("shelves") && value.at("shelves").isListValue()) {
        Value retval({});
        value.at("shelves").const_list_for_each([&projectDir, &retval](auto v) {
            auto shelfpath = v.at("path").stringValue();
            if (shelfpath.find("/") != 0) {
                shelfpath = projectDir + shelfpath;
            }
            logger::trace(" - include {}", shelfpath);
            auto shelfDir = shelfpath.substr(0, shelfpath.rfind("/")+1);
            std::map<std::string, std::string> env_dictionary;
            env_dictionary["${ShelfDirectory}"] = shelfDir;
            //logger::trace(" - ShelfDirectory = {}", shelfDir);
            auto v2 = replaceAndCopy(parseShelf(shelfpath, shelfDir), env_dictionary);
            //logger::trace(" - retval = {}", retval);
            //logger::trace(" - value = {}", v2);
            retval.operator=(merge(retval, v2));
            //logger::trace(" - merged retval = {}", retval);
        });
        //logger::trace(" - retval before merge = {}", retval);
        //logger::trace(" - value  before merge = {}", value);
        auto v3 = merge(value, retval);
        //logger::trace(" - merged value v3 = {}", v3);
        logger::trace("ProcessConfigParser::parseShelves(projectDir={}) exit", projectDir);
        return v3;
    }
    logger::trace("ProcessConfigParser::parseShelves(projectDir={}) exit", projectDir);
    return value;
}
 
Value ProcessConfigParser::parseShelf(const std::string& shelfFilePath, const std::string& projectDir) {
    logger::trace("ProcessConfigParser::parseShelf({})", shelfFilePath);
    std::map<std::string, std::string> env_dictionary;
    env_dictionary["${ShelfDirectory}"] = projectDir;
    std::filesystem::path _path(shelfFilePath);
    if (_path.extension() == ".nkshelf") {
        auto fp = fopen(shelfFilePath.c_str(), "r");
        if (fp == nullptr) {
            logger::error("ProcessConfigParser::parseShelf failed. File is " + shelfFilePath);  
            logger::trace("ProcessConfigParser::parseShelf exit");
            return Value({});
        }
        // auto v = juiz::json::toValue(fp);
        // logger::trace(" - ShelfDirectory = {}", projectDir);
        auto v = replaceAndCopy(juiz::json::toValue(fp), env_dictionary);
        // logger::info("ProcessConfigParser::parseShelf success. File is {}.", shelfFilePath);
        // logger::trace("ProcessConfigParser::parseShelf exit");
        return parseShelves(v, projectDir);
    } else if (_path.extension() == ".jshelf") {
        auto v = replaceAndCopy(juiz::yaml::toValue(std::ifstream(_path)), env_dictionary);
        logger::trace("ProcessConfigParser::parseSubProject(projFilePath={}) exit", shelfFilePath);
        return parseSubProjects(v, projectDir);
    }
    return Value::error(logger::trace("ProcessConfigParser::parseShelf({}) failed. File extension must be .nkshelf or .jshelf", projectDir));   
}

Value ProcessConfigParser::parseConfig(std::FILE* fp, const std::string& filepath, const std::string& shelfPath/*=""*/) {
    if (fp == nullptr) {
        logger::warn("ProcessConfigParser::parseConfig failed. File pointer is NULL.");
        return Value({});
    }
    auto projectDir = filepath.substr(0, filepath.rfind("/")+1);
    auto v = juiz::json::toValue(fp);
    v["projectDir"] = 
    v["projectFilePath"] = filepath;
    auto conf = parseShelves(v, projectDir);
    v["projectFilePath"] = filepath;
    return conf;
}

Value ProcessConfigParser::parseConfig(const std::string& jsonStr) {
    return (juiz::json::toValue(jsonStr));
}