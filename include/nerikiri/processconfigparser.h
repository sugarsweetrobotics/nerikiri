#pragma once

#include <cstdio>

#include "nerikiri/value.h"
#include "nerikiri/datatype/json.h"
#include "nerikiri/util/logger.h"

namespace nerikiri {



    class ProcessConfigParser {
    public:


    public:
      ProcessConfigParser();

    public:
      static Value parseConfig(std::FILE* fp, const std::string& filepath) {
        if (fp == nullptr) {
          logger::warn("ProcessConfigParser::parseConfig failed. File pointer is NULL.");
          return Value({});
        }
        auto v = nerikiri::json::toValue(fp);
        v["projectFilePath"] = filepath;
        return parseShelves(v);
      }

      static Value parseConfig(const std::string& jsonStr) {
          return parseShelves(nerikiri::json::toValue(jsonStr));
      }

    private:
      static Value parseShelves(const Value& value) {
        std::string fullpath = value.at("projectFilePath").stringValue();
        fullpath = fullpath.substr(0, fullpath.rfind("/")+1);
        if (value.hasKey("shelves") && value.at("shelves").isListValue()) {
          Value retval({});
          value.at("shelves").list_for_each([fullpath, &retval](auto v) {
            auto shellpath = fullpath + v.at("path").stringValue();
            retval.operator=(merge(retval, parseConfig(fopen(shellpath.c_str(), "r"), shellpath)));
          });
          return merge(retval, value);
        }
        return value;
      }
    };
}