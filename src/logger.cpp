#include <iostream>

#include "nerikiri/logger.h"
#include <regex>

using namespace nerikiri;
using namespace nerikiri::logger; 

void nerikiri::logger::setLogLevel(const LOG_LEVEL& level) {
}

LOG_LEVEL nerikiri::logger::getLogLevel() {
  return FATAL;
}

format_type nerikiri::logger::formatter(format_type&& fmt, const LOG_LEVEL& severity) {
  return std::string("[") + to_string(severity) + "] " + fmt;
}

format_type nerikiri::logger::formatter(format_type&& fmt, const int32_t& arg) {
  return std::regex_replace(fmt, std::regex("\\{[^\\}]*\\}"), std::to_string(arg) , std::regex_constants::format_first_only);
}

format_type nerikiri::logger::formatter(format_type&& fmt, const double& arg) {
  return std::regex_replace(fmt, std::regex("\\{[^\\}]*\\}"), std::to_string(arg) , std::regex_constants::format_first_only);
}

format_type nerikiri::logger::formatter(format_type&& fmt, const std::string& arg) {
  return std::regex_replace(fmt, std::regex("\\{[^\\}]*\\}"), arg, std::regex_constants::format_first_only);
}

void nerikiri::logger::log(std::string&& fmt) {
  std::cout << fmt << std::endl;
}
