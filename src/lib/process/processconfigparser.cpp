#include <filesystem>
#include <iostream>

#include <juiz/utils/yaml.h>
#include "processconfigparser.h"


using namespace juiz;
namespace fs = std::filesystem;


  /**
   * ディクショナリのキーにあるパターンでマッチした部分をディクショナリの値で置き換えるツール
   * @param ss 置き換え元の文字列
   * @param dictionary 置き換えディクショナリ．{"hoge", "foo"}というディクショナリならばhogeというパターンはfooに全て置き換わる
   * @return 置き換え後の文字列
   */
  static std::string stringReplace(const std::string& ss, const std::map<std::string, std::string>& dictionary) {
    auto s = ss;
    juiz::foreach<std::string, std::string>(dictionary, [&s](const std::string& k, const std::string& v) {
        s = replaceAll(s, k, v);
    });
    return s;
  }

  /**
   * Value型の値をディクショナリのキーにあるパターンでマッチした部分をディクショナリの値で置き換えるツール
   * ValueがString型であれば置き換えを試みる．オブジェクト型のキーに対しては置き換えを行わない
   * @param value 置き換え元のデータ
   * @param dictionary 置き換えディクショナリ．{"hoge", "foo"}というディクショナリならばhogeというパターンはfooに全て置き換わる
   * @return 置き換え後の文字列
   */
  Value juiz::replaceAndCopy(const Value& value, const std::map<std::string, std::string>& dictionary) {
    if (value.isObjectValue()) {
      Value v({});
      value.const_object_for_each([&v, &dictionary](auto& key, auto& cvalue) {
        v[key] = replaceAndCopy(cvalue, dictionary);
      });
      return v;
    }
    if (value.isListValue()) {
      Value v = Value::list();
      value.const_list_for_each([&v, &dictionary](auto& cvalue) {
        v.push_back(replaceAndCopy(cvalue, dictionary));
      });
      return v;
    }
    if (value.isStringValue()) {
      return stringReplace(value.stringValue(), dictionary);
    }

    return value;
  }


namespace {
Value parseSubProjects(const Value& value, const std::filesystem::path& projectDir, std::map<std::string, std::string>& env_dictionary);

Value parseSubProject(const fs::path& projFilePath, const fs::path& projectDir, std::map<std::string, std::string>& env_dictionary);

Value parseShelves(const Value& value, const fs::path& projectDir, const Value& load_paths, std::map<std::string, std::string>& env_dictionary);

Value parseShelf(const fs::path& shelfFilePath, const fs::path& projectDir, std::map<std::string, std::string>& env_dictionary);

Value parseConfig(std::FILE* fp, const std::string& filepath, const std::string& shelfPath="");

Value parseConfig(const std::string& jsonStr);



}
  
namespace {
  std::filesystem::path getProjectDir(const std::filesystem::path& path) {
    if (!path.is_absolute()) { /// 絶対パス指定ではなかった
        return fs::current_path() / path.parent_path();
    }
    return path.parent_path();
  }

  bool isNKProj(const std::filesystem::path& projectFilePath) {
      return projectFilePath.extension() == ".nkproj" || projectFilePath.extension() == ".nkshelf";
  }

  Value loadNKProj(const std::filesystem::path& projectFilePath) {
    logger::trace("- opening NKPROJ file.");
    // 拡張子が.nkprojであればjsonを仮定
    auto fp = fopen(projectFilePath.c_str(), "r");
    if (fp == nullptr) {
        logger::warn("ProcessConfigParser::parseConfig failed. Can not open file.  File path is {}.", projectFilePath);
        return Value({});
    }
    return juiz::json::toValue(fp);
  }

  Value loadJProj(const std::filesystem::path& projectFilePath) {
    logger::trace("- opening JPROJ file {}", projectFilePath);
    std::ifstream src_f(projectFilePath);
    return juiz::yaml::toValue(src_f);
  }

  Value loadConfig(const std::filesystem::path& projectFilePath) {
    if (isNKProj(projectFilePath)) return loadNKProj(projectFilePath);
    auto config = loadJProj(projectFilePath);
    config["projectDir"] = getProjectDir(projectFilePath).string();
    config["projectFilePath"] = projectFilePath.string();
    return config;
  }

  
}

Value ProcessConfigParser::parseProjectFile(const std::filesystem::path& projectFilePath) {
    logger::trace("ProcessConfigParser::parseProjectFile({}) called", projectFilePath);
    const auto projectDir = getProjectDir(projectFilePath);
    std::map<std::string, std::string> env_dictionary {
        {"${ProjectDirectory}", projectDir}
    };

    const Value config0 = replaceAndCopy(loadConfig(projectFilePath), env_dictionary);
    logger::trace("- parsing sub projects....");
    const auto config1 = parseSubProjects(config0, projectDir, env_dictionary);
    logger::trace("- parsing shelves....");
    const auto config2 = parseShelves(config1, projectDir, config1["load_paths"], env_dictionary);
    logger::trace("ProcessConfigParser::parseProjectFile({}) exit", projectFilePath);
    return config2;
}



namespace {
Value parseSubProjects(const Value& value, const std::filesystem::path& projectDir, std::map<std::string, std::string>& env_dictionary) {
    logger::trace("ProcessConfigParser::parseSubProjects(projectDir={}) called", projectDir);
    // includesタグにしたがってサブプロジェクトを読み込む
    if (!value.hasKey("includes") || !value.at("includes").isListValue()) {
        return value;
    }

    Value retval({});
    value["includes"].const_list_for_each([&env_dictionary, &projectDir, &retval](auto v) {
        fs::path subProjPath(v["path"].stringValue());
        if (!subProjPath.is_absolute()) {
            subProjPath = projectDir / subProjPath;
        }
        env_dictionary["${SubProjectDirectory}"] = subProjPath.parent_path().string();
        retval.operator=(merge(retval, replaceAndCopy(parseSubProject(subProjPath, projectDir, env_dictionary), env_dictionary)));
    });
    logger::trace("ProcessConfigParser::parseSubProjects(projectDir={}) exit", projectDir);
    return merge(retval, value);
}

Value parseSubProject(const fs::path& projFilePath, const fs::path& projectDir, std::map<std::string, std::string>& env_dictionary) {
    logger::trace("ProcessConfigParser::parseSubProject(projFilePath={})", projFilePath);
    return parseSubProjects(replaceAndCopy(loadConfig(projFilePath), env_dictionary), projectDir, env_dictionary);
}


fs::path findFileWithLoadPaths(const fs::path& filePath, const fs::path& projectDir, const Value& load_paths) {
    if (filePath.is_absolute()) {
        if (std::filesystem::exists(filePath)) {
            return filePath;
        }
    }

    fs::path shelfpath = projectDir / filePath;
    if (std::filesystem::exists(shelfpath)) {
        return shelfpath;
    }

    for(const Value& p : load_paths.listValue()) {
        logger::trace("-- load_path({}) searching....", p.stringValue());
        shelfpath = p.stringValue() / filePath;
        if (fs::exists(shelfpath)) {
            return shelfpath;
        }
    }
    
    return {}; /// empty path
}

Value parseShelves(const Value& value, const fs::path& projectDir, const Value& load_paths, std::map<std::string, std::string>& env_dictionary) {
    logger::trace("ProcessConfigParser::parseShelves(projectDir={}) called", projectDir);
    if (!value.hasKey("shelves") || !value["shelves"].isListValue()) {
        logger::trace("ProcessConfigParser::parseShelves(projectDir={}) exit", projectDir);
        return value;
    }

    Value retval({});
    value["shelves"].const_list_for_each([&projectDir, &retval, &load_paths, &env_dictionary](auto v) {
        const auto shelfpath = findFileWithLoadPaths(v["path"].stringValue(), projectDir,load_paths);
        if (shelfpath.empty()) {
            logger::error("ProcessConfigParser::parseShelves(projectDir={}) processconfigparser.parseShelves failed to include shelf ('path'='{}'", v["path"]);
            return;
        }

        logger::trace("- include {}", shelfpath);
        auto shelfDir = shelfpath.parent_path();
        env_dictionary["${ShelfDirectory}"] = shelfDir;
        auto v2 = replaceAndCopy(parseShelf(shelfpath, shelfDir, env_dictionary), env_dictionary);
        retval.operator=(merge(retval, v2));
    
    });
    const auto v3 = merge(value, retval);
    logger::trace("ProcessConfigParser::parseShelves(projectDir={}) exit", projectDir);
    return v3;

}
 
Value parseShelf(const fs::path& shelfFilePath, const fs::path& projectDir, std::map<std::string, std::string>& env_dictionary) {
    logger::trace("ProcessConfigParser::parseShelf({})", shelfFilePath);
    auto v = replaceAndCopy(loadConfig(shelfFilePath), env_dictionary);
    return parseShelves(v, projectDir, v["load_paths"], env_dictionary);
}

Value parseConfig(std::FILE* fp, const std::string& filepath, const std::string& shelfPath/*=""*/) {
    if (fp == nullptr) {
        logger::warn("ProcessConfigParser::parseConfig failed. File pointer is NULL.");
        return Value({});
    }
    auto projectDir = filepath.substr(0, filepath.rfind("/")+1);
    auto v = juiz::json::toValue(fp);
    v["projectFilePath"] = filepath;
    std::map<std::string, std::string> dic{};
    auto conf = parseShelves(v, projectDir, {}, dic);
    v["projectFilePath"] = filepath;
    return conf;
}

Value parseConfig(const std::string& jsonStr) {
    return (juiz::json::toValue(jsonStr));
}

}
