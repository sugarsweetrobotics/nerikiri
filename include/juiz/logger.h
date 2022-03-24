#pragma once

#include <string>
#include <juiz/juiz.h>
#include <juiz/value.h>

namespace juiz::logger {

  enum LOG_LEVEL { 
    LOG_VERBOSE,
    LOG_TRACE3,
    LOG_TRACE2,
    LOG_TRACE1,
    LOG_TRACE,
    LOG_DEBUG,
    LOG_NORMAL,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
    LOG_OFF
  };

  inline std::string to_string(const LOG_LEVEL& level) {
    switch(level) {
    case LOG_VERBOSE:return "VERBOSE";
    case LOG_TRACE3: return " TRACE3";
    case LOG_TRACE2: return " TRACE2";
    case LOG_TRACE1: return " TRACE1";
    case LOG_TRACE:  return " TRACE ";
    case LOG_DEBUG:  return " DEBUG ";
    case LOG_INFO:   return " INFO  ";
    case LOG_NORMAL: return "NORMAL ";
    case LOG_WARN:   return " WARN  ";
    case LOG_ERROR:  return " ERROR ";
    case LOG_FATAL:  return " FATAL ";
    case LOG_OFF:    return  " OFF   ";
    default:   return  "UNKNOWN";
    }
  }
  
  inline LOG_LEVEL from_string(const std::string& level) {
    if (level == "VERBOSE") {
      return LOG_VERBOSE;
    } else if (level == "TRACE3") {
      return LOG_TRACE3;
    } else if (level == "TRACE2") {
      return LOG_TRACE2;
    } else if (level == "TRACE1") {
      return LOG_TRACE1;
    } else if (level == "TRACE") {
      return LOG_TRACE;
    } else if (level == "DEBUG") { 
      return LOG_DEBUG;
    } else if (level == "INFO") {
      return LOG_INFO;
    } else if (level == "NORMAL") {
      return LOG_NORMAL;
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

  inline bool isDoLog(const LOG_LEVEL& lvl) {
    return getLogLevel() <= lvl;
  }

  NK_API bool initLogger();

  using format_type = std::string;

  NK_API format_type formatter(format_type&& fmt, const LOG_LEVEL& severity);
  
  NK_API format_type formatter(format_type&& fmt, const int32_t& arg);

  NK_API format_type formatter(format_type&& fmt, const int64_t& arg);

  NK_API format_type formatter(format_type&& fmt, const double& arg);

  NK_API format_type formatter(format_type&& fmt, const std::string& arg);

  inline NK_API format_type formatter(format_type&& fmt, const char* arg) {
    return formatter(std::move(fmt), std::string(arg));
  }

  inline NK_API format_type formatter(format_type&& fmt, const juiz::Value& arg) {
    return formatter(std::move(fmt), juiz::str(arg));
  }
  

  //template<typename T>
  //  format_type formatter(format_type&& fmt, const T& arg) {
  //    return formatter(std::forward<format_type>(fmt), juiz::str(arg));
  //  }

  NK_API std::string log(const LOG_LEVEL& severity, std::string&& fmt);
  inline std::string doNotLog(const LOG_LEVEL& severity, std::string&& fmt) { return std::move(fmt); }

  template<typename T, typename... Args>
  inline std::string log(const LOG_LEVEL& severity, format_type&& fmt, const T& arg, const Args &... args) {
    return log(severity, formatter(std::forward<format_type>(fmt), arg), args...);
  }
 
  //template<typename... Args>
  //inline std::string log(format_type&& fmt, const Value& arg, const Args &... args) {
  //  return log(formatter(std::forward<format_type>(fmt), str(arg)), args...);
  // }

  template<typename T, typename... Args>
  inline std::string doNotLog(const LOG_LEVEL& severity, format_type&& fmt, const T& arg, const Args &... args) {
    return doNotLog(severity, formatter(std::forward<format_type>(fmt), arg), args...);
  }

  template<typename... Args>
  inline std::string doNotLog(const LOG_LEVEL& severity, format_type&& fmt, const Value& arg, const Args &... args) {
    return doNotLog(severity, formatter(std::forward<format_type>(fmt), std::string("object::Value")), args...);
  }
    
  template<typename... Args>
  inline std::string log(const LOG_LEVEL& severity, const char* fmt, const Args &... args) {
    if (!isDoLog(severity)) return doNotLog(severity, formatter(std::forward<std::string>(std::string(fmt)), severity), args...);
    return log(severity, formatter(std::forward<std::string>(std::string(fmt)), severity), args...);
  }

  template<typename... Args>
  inline std::string verbose(const char* fmt, const Args &... args) {
    return log(LOG_VERBOSE, fmt, args...);
  }

  template<typename... Args>
  inline std::string trace(const char* fmt, const Args &... args) {
    return log(LOG_TRACE, fmt, args...);
  }

  inline std::string trace(const std::string& str) {
    return log(LOG_TRACE, str.c_str());
  }

  template<typename... Args>
  inline std::string trace1(const char* fmt, const Args &... args) {
    return log(LOG_TRACE1, fmt, args...);
  }

  inline std::string trace1(const std::string& str) {
    return log(LOG_TRACE1, str.c_str());
  }

  template<typename... Args>
  inline std::string trace2(const char* fmt, const Args &... args) {
    return log(LOG_TRACE2, fmt, args...);
  }

  inline std::string trace2(const std::string& str) {
    return log(LOG_TRACE2, str.c_str());
  }

  template<typename... Args>
  inline std::string trace3(const char* fmt, const Args &... args) {
    return log(LOG_TRACE3, fmt, args...);
  }

  inline std::string trace3(const std::string& str) {
    return log(LOG_TRACE3, str.c_str());
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

  template<LOG_LEVEL LL>
  class log_object {
    std::string functionName_;
  public:
    
    template<typename... Args>
    log_object(const char* fmt, const Args &... args) {
      functionName_ = doNotLog(LL, fmt, args...);
      log(LL, (functionName_ + " entry").c_str());
    }

    log_object(const std::string& fmt) {
      functionName_ = fmt;
      log(LL, (functionName_ + " entry").c_str());
    }

    ~log_object() {
      log(LL, (functionName_ + " exit").c_str());
    }
  };

  using trace3_object = log_object<LOG_TRACE3>;
  using trace2_object = log_object<LOG_TRACE2>;
  using trace1_object = log_object<LOG_TRACE1>;
  using trace_object = log_object<LOG_TRACE>;
  using debug_object = log_object<LOG_DEBUG>;
  using info_object = log_object<LOG_INFO>;
  
}
