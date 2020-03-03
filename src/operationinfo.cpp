#include "nerikiri/operationinfo.h"
#include <sstream>
#include <string>

std::string nerikiri::str(const OperationInfo& info) {
  std::stringstream ss;
  ss << "OperationInfo(";
  ss << "name=" << info.at("name").stringValue();
  ss << ")";
  return ss.str();
}
