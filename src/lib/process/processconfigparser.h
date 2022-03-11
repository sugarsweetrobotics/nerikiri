#pragma once

#include <cstdio>
#include <filesystem>

#include <juiz/value.h>
#include <juiz/utils/json.h>
#include <juiz/logger.h>
#include <juiz/utils/functional.h>
#include <juiz/utils/os.h>
#include <juiz/utils/stringutil.h>

namespace juiz {

  /**
   * Value型の値をディクショナリのキーにあるパターンでマッチした部分をディクショナリの値で置き換えるツール
   * ValueがString型であれば置き換えを試みる．オブジェクト型のキーに対しては置き換えを行わない
   * @param value 置き換え元のデータ
   * @param dictionary 置き換えディクショナリ．{"hoge", "foo"}というディクショナリならばhogeというパターンはfooに全て置き換わる
   * @return 置き換え後の文字列
   */
  Value replaceAndCopy(const Value& value, const std::map<std::string, std::string>& dictionary);
  
  /**
   * プロジェクトファイルなどを読み込むヘルパークラス
   * ステートレスクラス
   */
  class ProcessConfigParser {
  public:
    ProcessConfigParser();
    static Value parseProjectFile(const std::filesystem::path& projectFilePath);
  };
}