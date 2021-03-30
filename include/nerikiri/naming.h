#pragma once

#include <string>
#include <vector>
#include <typeinfo>

#include <nerikiri/stringutil.h>
#ifdef __APPLE__
#include <cxxabi.h>
#elif WIN32
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")
#else
#include <cxxabi.h>

#endif

#include <nerikiri/value.h>

namespace nerikiri {

  static const auto name_separator = ':';

  namespace naming {
    inline const std::string join(const std::string& ns, const std::string& n) {
      return ns + name_separator + n;
    }

    inline const std::string join(const std::string& ns, const std::string& n, const std::string& n2) {
      return ns + name_separator + n + name_separator + n2;
    }

    inline std::pair<std::string, std::string> splitContainerAndOperationName(const std::string& name) {
      auto i = name.rfind(name_separator);
      if (i == std::string::npos) {
          return {name.substr(0, i), ""};
      }
      return {name.substr(0, i), name.substr(i+1)};
    }

  }


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

  inline std::vector<std::string> separateNamingContext(const std::string& name) {
      return stringSplit(name, name_separator);
  }

  inline std::pair<std::string, std::string> splitContainerAndOperationName(const std::string& name) {
      auto i = name.rfind(name_separator);
      if (i == std::string::npos) {
          return {name.substr(0, i), ""};
      }
      return {name.substr(0, i), name.substr(i+1)};
  }


  std::pair<std::string, std::string> separateNamespaceAndInstanceName(const std::string& fullName);


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


  inline bool nameValidator(const std::string& name) {
    if (name.find("/") != std::string::npos) {
      return false; // Invalid Name
    }
    if (name.find(":") != std::string::npos) {
      return false; // Invalid Name
    }
    return true;
  };

  bool operationValidator(const Value& opinfo);


}