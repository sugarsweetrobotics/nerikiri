#pragma once

#include <string>
#include <nerikiri/nerikiri.h>
#include <nerikiri/value.h>

namespace nerikiri::logger {

  enum LOG_LEVEL { LOG_TRACE,
      LOG_DEBUG,
      LOG_INFO,
      LOG_WARN,
      LOG_ERROR,
      LOG_FATAL,
      LOG_OFF };

  inline std::string to_string(const LOG_LEVEL& level) {
    switch(level) {
    case LOG_TRACE: return " TRACE ";
    case LOG_DEBUG: return " DEBUG ";
    case LOG_INFO:  return " INFO  ";
    case LOG_WARN:  return " WARN  ";
    case LOG_ERROR: return " ERROR ";
    case LOG_FATAL: return " FATAL ";
    case LOG_OFF:  return  " OFF   ";
    default:   return  "UNKNOWN";
    }
  }
  
  inline LOG_LEVEL from_string(const std::string& level) {
    if (level == "TRACE") {
      return LOG_TRACE;
    } else if (level == "DEBUG") { 
      return LOG_DEBUG;
    } else if (level == "INFO") {
      return LOG_INFO;
    } else if (level == "WARN") {
      return LOG_WARN;
    } else if (level == "ERROR") {
      return LOG_ERROR;
    } else if (level == "FATAL") {
      return LOG_FATAL;
    } else if (level == "OFF") {
      return LOG_OFF;
    } 
    return LOG_INFO;
  }
  

  NK_API void setLogLevel(const LOG_LEVEL& level);

  inline void setLogLevel(const std::string& level) { setLogLevel(from_string(level)); }

  NK_API LOG_LEVEL getLogLevel();

  NK_API void setLogFileName(const std::string& fileName);

  inline bool doLog(const LOG_LEVEL& lvl) {
    return getLogLevel() <= lvl;
  }

  NK_API bool initLogger();

  using format_type = std::string;

  NK_API format_type formatter(format_type&& fmt, const LOG_LEVEL& severity);
  
  NK_API format_type formatter(format_type&& fmt, const int32_t& arg);

  NK_API format_type formatter(format_type&& fmt, const double& arg);

  NK_API format_type formatter(format_type&& fmt, const std::string& arg);

  inline NK_API format_type formatter(format_type&& fmt, const char* arg) {
    return formatter(std::move(fmt), std::string(arg));
  }

  inline NK_API format_type formatter(format_type&& fmt, const nerikiri::Value& arg) {
    return formatter(std::move(fmt), nerikiri::str(arg));
  }
  

  //template<typename T>
  //  format_type formatter(format_type&& fmt, const T& arg) {
  //    return formatter(std::forward<format_type>(fmt), nerikiri::str(arg));
  //  }

  NK_API std::string log(std::string&& fmt);
  inline std::string doNotLog(std::string&& fmt) { return std::move(fmt); }

  template<typename T, typename... Args>
  inline std::string log(format_type&& fmt, const T& arg, const Args &... args) {
    return log(formatter(std::forward<format_type>(fmt), arg), args...);
  }
 
  //template<typename... Args>
  //inline std::string log(format_type&& fmt, const Value& arg, const Args &... args) {
  //  return log(formatter(std::forward<format_type>(fmt), str(arg)), args...);
  // }

  template<typename T, typename... Args>
  inline std::string doNotLog(format_type&& fmt, const T& arg, const Args &... args) {
    return doNotLog(formatter(std::forward<format_type>(fmt), arg), args...);
  }

  template<typename... Args>
  inline std::string doNotLog(format_type&& fmt, const Value& arg, const Args &... args) {
    return doNotLog(formatter(std::forward<format_type>(fmt), std::string("object::Value")), args...);
  }
    
  template<typename... Args>
  inline std::string log(const LOG_LEVEL& severity, const char* fmt, const Args &... args) {
    if (!doLog(severity)) return doNotLog(formatter(std::forward<std::string>(std::string(fmt)), severity), args...);
    return log(formatter(std::forward<std::string>(std::string(fmt)), severity), args...);
  }

  template<typename... Args>
  inline std::string trace(const char* fmt, const Args &... args) {
    return log(LOG_TRACE, fmt, args...);
  }

  template<typename... Args>
  inline std::string debug(const char* fmt, const Args &... args) {
    return log(LOG_DEBUG, fmt, args...);
  }

  template<typename... Args>
  inline std::string info(const char* fmt, const Args &... args) {
    return log(LOG_INFO, fmt, args...);
  }

  inline std::string warn(const std::string& str) {
    return log(LOG_WARN, str.c_str());
  }

  template<typename... Args>
  inline std::string warn(const char* fmt, const Args &... args) {
    return log(LOG_WARN, fmt, args...);
  }

  inline std::string error(const std::string& str) {
    return log(LOG_ERROR, str.c_str());
  }

  template<typename... Args>
  inline std::string error(const char* fmt, const Args &... args) {
    return log(LOG_ERROR, fmt, args...);
  }

  inline std::string fatal(const std::string& str) {
    return log(LOG_FATAL, str.c_str());
  }

  template<typename... Args>
  inline std::string fatal(const char* fmt, const Args &... args) {
    return log(LOG_FATAL, fmt, args...);
  }

}
