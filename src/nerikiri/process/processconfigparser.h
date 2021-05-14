#pragma once

#include <cstdio>

#include "nerikiri/value.h"
#include "nerikiri/utils/json.h"
#include "nerikiri/logger.h"
#include "nerikiri/utils/functional.h"
#include "nerikiri/utils/os.h"
#include "nerikiri/utils/stringutil.h"

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

  /**
   * プロジェクトファイルなどを読み込むヘルパークラス
   * ステートレスクラス
   */
  class ProcessConfigParser {
  public:


  public:
    ProcessConfigParser();

  public:

    static Value parseProjectFile(const std::string& projectFilePath);

  private:

    static Value parseSubProjects(const Value& value, const std::string& projectDir);

    static Value parseSubProject(const std::string& projFilePath, const std::string& projectDir);

    static Value parseShelves(const Value& value, const std::string& projectDir);
 
    static Value parseShelf(const std::string& shelfFilePath, const std::string& projectDir);

    static Value parseConfig(std::FILE* fp, const std::string& filepath, const std::string& shelfPath="");
  public:
    static Value parseConfig(const std::string& jsonStr);

  private:
  };
}