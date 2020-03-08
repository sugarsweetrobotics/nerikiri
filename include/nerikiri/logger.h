#pragma once

#include <string>

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

  //template<typename T>
  //  format_type formatter(format_type&& fmt, const T& arg) {
  //    return formatter(std::forward<format_type>(fmt), nerikiri::str(arg));
  //  }

  void log(std::string&& fmt);

  template<typename T, typename... Args>
  inline void log(format_type&& fmt, const T& arg, const Args &... args) {
    log(formatter(std::forward<format_type>(fmt), arg), args...);
  }
    
  template<typename... Args>
  inline void log(const LOG_LEVEL& severity, const char* fmt, const Args &... args) {
    if (!doLog(severity)) return;
    log(formatter(std::forward<std::string>(std::string(fmt)), severity), args...);
  }

  template<typename... Args>
  inline void trace(const char* fmt, const Args &... args) {
    log(TRACE, fmt, args...);
  }

  template<typename... Args>
  inline void debug(const char* fmt, const Args &... args) {
    log(DEBUG, fmt, args...);
  }

  template<typename... Args>
  inline void info(const char* fmt, const Args &... args) {
    log(INFO, fmt, args...);
  }

  inline void warn(const std::string& str) {
    log(WARN, str.c_str());
  }

  template<typename... Args>
  inline void warn(const char* fmt, const Args &... args) {
    log(WARN, fmt, args...);
  }

  inline void error(const std::string& str) {
    log(ERROR, str.c_str());
  }

  template<typename... Args>
  inline void error(const char* fmt, const Args &... args) {
    log(ERROR, fmt, args...);
  }

  template<typename... Args>
  inline void fatal(const char* fmt, const Args &... args) {
    log(FATAL, fmt, args...);
  }

}
