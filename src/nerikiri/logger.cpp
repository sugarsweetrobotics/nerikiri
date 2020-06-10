#include <iostream>
#include <regex>
#include <chrono>

#include "nerikiri/logger.h"

using namespace nerikiri;
using namespace nerikiri::logger; 

static LOG_LEVEL g_loglevel = logger::LOG_INFO;
static auto startTime = std::chrono::system_clock::now();

void nerikiri::logger::setLogLevel(const LOG_LEVEL& level) {
  g_loglevel = level;
}

LOG_LEVEL nerikiri::logger::getLogLevel() {
  return g_loglevel;
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

std::string nerikiri::logger::log(std::string&& fmt) {
  std::cout << "[" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - startTime).count() << "] " << fmt << std::endl;
  return std::move(fmt);
}
