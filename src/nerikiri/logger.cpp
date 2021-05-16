#include <iostream>
#include <regex>
#include <chrono>
#include <fstream>
#include <juiz/logger.h>

using namespace juiz;
using namespace juiz::logger; 

static LOG_LEVEL g_loglevel = logger::LOG_INFO;
static auto startTime = std::chrono::system_clock::now();
static std::string g_logFileName = "nerikiri.log";
static bool init = false;
static std::ofstream g_logFile;

void juiz::logger::setLogLevel(const LOG_LEVEL& level) {
  g_loglevel = level;
}

LOG_LEVEL juiz::logger::getLogLevel() {
  return g_loglevel;
}

void juiz::logger::setLogFileName(const std::string& fileName) {
  g_logFileName = fileName;
}

bool juiz::logger::initLogger() {
  g_logFile = std::ofstream(g_logFileName);
  init = true;
  return true;
}

format_type juiz::logger::formatter(format_type&& fmt, const LOG_LEVEL& severity) {
  return std::string("[") + to_string(severity) + "] " + fmt;
}

format_type juiz::logger::formatter(format_type&& fmt, const int32_t& arg) {
  return std::regex_replace(fmt, std::regex("\\{[^\\}]*\\}"), std::to_string(arg) , std::regex_constants::format_first_only);
}

format_type juiz::logger::formatter(format_type&& fmt, const double& arg) {
  return std::regex_replace(fmt, std::regex("\\{[^\\}]*\\}"), std::to_string(arg) , std::regex_constants::format_first_only);
}

format_type juiz::logger::formatter(format_type&& fmt, const std::string& arg) {
  return std::regex_replace(fmt, std::regex("\\{\\}"), arg, std::regex_constants::format_first_only);
//  return std::regex_replace(fmt, std::regex("\\{[^\\}]*\\}"), arg, std::regex_constants::format_first_only);
}

std::string juiz::logger::log(std::string&& fmt) {
  std::cout << "[" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - startTime).count() << "] " << fmt << std::endl;
  if (init) {
    g_logFile << "[" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - startTime).count() << "] " << fmt << std::endl;
  }
  return std::move(fmt);
}
