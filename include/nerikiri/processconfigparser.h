#pragma once

#include <cstdio>

#include "nerikiri/value.h"
#include "nerikiri/json.h"
#include "nerikiri/logger.h"
#include "nerikiri/stringutil.h"

namespace nerikiri {

  /**
   * ディクショナリのキーにあるパターンでマッチした部分をディクショナリの値で置き換えるツール
   * @param ss 置き換え元の文字列
   * @param dictionary 置き換えディクショナリ．{"hoge", "foo"}というディクショナリならばhogeというパターンはfooに全て置き換わる
   * @return 置き換え後の文字列
   */
  static std::string stringReplace(const std::string& ss, const std::map<std::string, std::string>& dictionary) {
    auto s = ss;
    nerikiri::foreach<std::string, std::string>(dictionary, [&s](const std::string& k, const std::string& v) {
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
  static Value replaceAndCopy(const Value& value, const std::map<std::string, std::string>& dictionary) {
    if (value.isObjectValue()) {
      Value v({});
      value.object_for_each([&v, &dictionary](auto& key, auto& cvalue) {
        v[key] = replaceAndCopy(cvalue, dictionary);
      });
      return v;
    }
    if (value.isListValue()) {
      Value v = Value::list();
      value.list_for_each([&v, &dictionary](auto& cvalue) {
        v.push_back(replaceAndCopy(cvalue, dictionary));
      });
      return v;
    }
    if (value.isStringValue()) {
      return stringReplace(value.stringValue(), dictionary);
    }

    return value;
  }

  /**
   * プロジェクトファイルなどを読み込むヘルパークラス
   * ステートレスクラス
   */
  class ProcessConfigParser {
  public:


  public:
    ProcessConfigParser();

  public:

    static Value parseProjectFile(const std::string& projectFilePath) {
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

  private:

    static Value parseSubProjects(const Value& value, const std::string& projectDir) {
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

    static Value parseSubProject(const std::string& projFilePath, const std::string& projectDir) {
      logger::trace("ProcessConfigParser::parseSubProject({})", projFilePath);
      auto fp = fopen(projFilePath.c_str(), "r");
      if (fp == nullptr) {
        logger::warn("ProcessConfigParser::parseSubProject failed. File is " + projFilePath);
        return Value({});
      }
      auto v = nerikiri::json::toValue(fp);
      return parseSubProjects(v, projectDir);
    }


    static Value parseShelves(const Value& value, const std::string& projectDir) {
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
 
    static Value parseShelf(const std::string& shelfFilePath, const std::string& projectDir) {
      logger::trace("ProcessConfigParser::parseShelf({})", shelfFilePath);
      auto fp = fopen(shelfFilePath.c_str(), "r");
      if (fp == nullptr) {
        logger::warn("ProcessConfigParser::parseShelf failed. File is " + shelfFilePath);
        return Value({});
      }
      auto v = nerikiri::json::toValue(fp);
      return parseShelves(v, projectDir);
    }

    static Value parseConfig(std::FILE* fp, const std::string& filepath, const std::string& shelfPath="") {
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
  public:
    static Value parseConfig(const std::string& jsonStr) {
        return (nerikiri::json::toValue(jsonStr));
    }

  private:
  };
}