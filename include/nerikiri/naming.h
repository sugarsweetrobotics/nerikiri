#pragma once

#include <string>
#include <vector>
#include <typeinfo>

#ifdef __APPLE__
#include <cxxabi.h>
#elif WIN32
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")
#else
#include <cxxabi.h>

#endif

namespace nerikiri {

    inline std::string demangle(const char *demangled) {
      int status;
      // for win32, use UnDecorateSymbolName function.
#ifdef __APPLE__
      return abi::__cxa_demangle(demangled, 0, 0, &status);
#elif WIN32
      char strbuf[1024];
      ::UnDecorateSymbolName(demangled, strbuf, 1024, UNDNAME_COMPLETE | UNDNAME_NO_ACCESS_SPECIFIERS | UNDNAME_NO_ALLOCATION_MODEL | UNDNAME_NO_SPECIAL_SYMS |
          UNDNAME_NO_MEMBER_TYPE | UNDNAME_NO_MS_KEYWORDS);
      auto retval = std::string(strbuf);
      if (retval.find("class") != std::string::npos) {
          return retval.substr(6);
      }
      if (retval.find("struct") != std::string::npos) {
          return retval.substr(7);
      }
      return retval;
#else
      return abi::__cxa_demangle(demangled, 0, 0, &status);
#endif

    }

    template<typename T>
    std::string numbering_policy(std::vector<T>& vec, const std::string& typeName, const std::string& ext) {
        int number = 0;
        bool foundFlag = false;
        std::string instanceName;
        do {
            instanceName = typeName + std::to_string(number) + ext;
            foundFlag = false;
            for(auto v : vec) {
                if (v->info().at("instanceName").stringValue() == instanceName ) {
                    foundFlag = true;
                    number++;
                    break;
                }
            }
        } while (foundFlag);
        return instanceName;
    }
}