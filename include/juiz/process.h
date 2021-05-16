#pragma once

#include <string>
#include <memory>

#include <juiz/juiz.h>
#include <juiz/value.h>
#include <juiz/process_api.h>
#include <juiz/client_proxy_api.h>

namespace juiz {
  
  std::shared_ptr<ProcessAPI> process(const std::string& name);

  std::shared_ptr<ProcessAPI> process(const int argc, const char** argv);

  std::shared_ptr<ProcessAPI> process(const std::string& name, const Value& config);

  std::shared_ptr<ProcessAPI> process(const std::string& name, const std::string& jsonStr);

}
