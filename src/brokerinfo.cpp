#include "nerikiri/brokerinfo.h"
#include <string>
#include <sstream>

std::string nerikiri::str(const BrokerInfo& info) {
  std::stringstream ss;
  ss << "BrokerInfo(";
  ss << "name=" << info.name;
  ss << ")";
  return ss.str();
}


