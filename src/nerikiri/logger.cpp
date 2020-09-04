#include <iostream>
#include <regex>
#include <chrono>
#include <fstream>
#include "nerikiri/logger.h"

using namespace nerikiri;
using namespace nerikiri::logger; 

static LOG_LEVEL g_loglevel = logger::LOG_INFO;
static auto startTime = std::chrono::system_clock::now();
static std::string g_logFileName = "nerikiri.log";
static bool init = false;
static std::ofstream g_logFile;

void nerikiri::logger::setLogLevel(const LOG_LEVEL& level) {
  g_loglevel = level;
}

LOG_LEVEL nerikiri::logger::getLogLevel() {
  return g_loglevel;
}

void nerikiri::logger::setLogFileName(const std::string& fileName) {
  g_logFileName = fileName;
}

bool nerikiri::logger::initLogger() {
  g_logFile = std::ofstream(g_logFileName);
  init = true;
  return true;
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
  return std::regex_replace(fmt, std::regex("\\{\\}"), arg, std::regex_constants::format_first_only);
//  return std::regex_replace(fmt, std::regex("\\{[^\\}]*\\}"), arg, std::regex_constants::format_first_only);
}

std::string nerikiri::logger::log(std::string&& fmt) {
  std::cout << "[" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - startTime).count() << "] " << fmt << std::endl;
  if (init) {
    g_logFile << "[" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - startTime).count() << "] " << fmt << std::endl;
  }
  return std::move(fmt);
}
