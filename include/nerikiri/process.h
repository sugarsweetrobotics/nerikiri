#pragma once

#include <string>
#include <memory>

#include <nerikiri/nerikiri.h>
#include <nerikiri/value.h>
#include <nerikiri/process_api.h>
#include <nerikiri/client_proxy_api.h>

namespace nerikiri {
  
  std::shared_ptr<ProcessAPI> process(const std::string& name);

  std::shared_ptr<ProcessAPI> process(const int argc, const char** argv);

  std::shared_ptr<ProcessAPI> process(const std::string& name, const Value& config);

  std::shared_ptr<ProcessAPI> process(const std::string& name, const std::string& jsonStr);

}
