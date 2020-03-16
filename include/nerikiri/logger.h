#pragma once

#include <string>
#include "nerikiri/value.h"

namespace nerikiri::logger {

  enum LOG_LEVEL { TRACE,
		   DEBUG,
		   INFO,
		   WARN,
		   ERROR,
		   FATAL,
		   OFF };

  inline std::string to_string(const LOG_LEVEL& level) {
    switch(level) {
    case TRACE: return " TRACE ";
    case DEBUG: return " DEBUG ";
    case INFO:  return " INFO  ";
    case WARN:  return " WARN  ";
    case ERROR: return " ERROR ";
    case FATAL: return " FATAL ";
    case OFF:  return  " OFF   ";
    default:   return  "UNKNOWN";
    }
  }
  

  void setLogLevel(const LOG_LEVEL& level);
  LOG_LEVEL getLogLevel();

  inline bool doLog(const LOG_LEVEL& lvl) {
    return getLogLevel() >= lvl;
  }

  using format_type = std::string;

  format_type formatter(format_type&& fmt, const LOG_LEVEL& severity);
  
  format_type formatter(format_type&& fmt, const int32_t& arg);

  format_type formatter(format_type&& fmt, const double& arg);

  format_type formatter(format_type&& fmt, const std::string& arg);

  /*
  format_type formatter(format_type&& fmt, const nerikiri::Value& arg) {
    return formatter(std::move(fmt), nerikiri::str(arg));
  }
  */

  //template<typename T>
  //  format_type formatter(format_type&& fmt, const T& arg) {
  //    return formatter(std::forward<format_type>(fmt), nerikiri::str(arg));
  //  }

  std::string log(std::string&& fmt);
  inline std::string doNotLog(std::string&& fmt) { return std::move(fmt); }

  template<typename T, typename... Args>
  inline std::string log(format_type&& fmt, const T& arg, const Args &... args) {
    return log(formatter(std::forward<format_type>(fmt), arg), args...);
  }

  template<typename T, typename... Args>
  inline std::string doNotLog(format_type&& fmt, const T& arg, const Args &... args) {
    return doNotLog(formatter(std::forward<format_type>(fmt), arg), args...);
  }
    
  template<typename... Args>
  inline std::string log(const LOG_LEVEL& severity, const char* fmt, const Args &... args) {
    if (!doLog(severity)) return doNotLog(formatter(std::forward<std::string>(std::string(fmt)), severity), args...);
    return log(formatter(std::forward<std::string>(std::string(fmt)), severity), args...);
  }


  template<typename... Args>
  inline std::string trace(const char* fmt, const Args &... args) {
    return log(TRACE, fmt, args...);
  }

  template<typename... Args>
  inline std::string debug(const char* fmt, const Args &... args) {
    return log(DEBUG, fmt, args...);
  }

  template<typename... Args>
  inline std::string info(const char* fmt, const Args &... args) {
    return log(INFO, fmt, args...);
  }

  inline std::string warn(const std::string& str) {
    return log(WARN, str.c_str());
  }

  template<typename... Args>
  inline std::string warn(const char* fmt, const Args &... args) {
    return log(WARN, fmt, args...);
  }

  inline std::string error(const std::string& str) {
    return log(ERROR, str.c_str());
  }

  template<typename... Args>
  inline std::string error(const char* fmt, const Args &... args) {
    return log(ERROR, fmt, args...);
  }

  inline std::string fatal(const std::string& str) {
    return log(FATAL, str.c_str());
  }

  template<typename... Args>
  inline std::string fatal(const char* fmt, const Args &... args) {
    return log(FATAL, fmt, args...);
  }

}
