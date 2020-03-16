#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/http/httpbroker.h"
#include "nerikiri/systemeditor.h"
using namespace std::literals::string_literals;

using namespace nerikiri;

int main(const int argc, const char* argv[]) {
  auto broker = nerikiri::http::brokerProxy("localhost", 8080);

  std::cout << nerikiri::str(broker->getBrokerInfo()) << std::endl;;
  //std::cout << nerikiri::str(broker->invokeOperationByName("increment")) << std::endl;;
  std::cout << nerikiri::str(broker->makeConnection(
    {
      {"name", "connection01"},
      {"provider", {
          {"info", {
            {"name", {"one"}}
          }},
          {"broker", {
            {"name", "HTTPBroker"},
            {"address", "localhost"},
            {"port", 8080}
          }}
        }},
      {"consumer", {
          {"info", {
            {"name", {"increment"}}
          }},
          {"broker", {
            {"name", "HTTPBroker"},
            {"address", "localhost"},
            {"port", 8080}
          }},
          {"target", {
              {"name", "arg01"}}
          }}
        }
    })) << std::endl;

  std::cout << nerikiri::str(broker->invokeOperationByName("increment")) << std::endl;;
  return 0;
}
