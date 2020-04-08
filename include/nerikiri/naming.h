#pragma once

#include <string>
#include <vector>
#include <typeinfo>
#include <cxxabi.h>

namespace nerikiri {

    inline std::string demangle(const char *demangled) {
      int status;
      // for win32, use UnDecorateSymbolName function.
      return abi::__cxa_demangle(demangled, 0, 0, &status);
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