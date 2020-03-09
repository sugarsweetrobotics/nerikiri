#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/http/httpbroker.h"
#include "nerikiri/systemeditor.h"
using namespace std::literals::string_literals;

using namespace nerikiri;

nerikiri::Operation increment( 
       {{"name", "increment"},
          {"defaultArg", {
            {"arg01", 0}
          }}
        },
		   [](auto arg) { 
		     arg["arg01"] = arg["arg01"].intValue() + 1;
		     return arg;
		   }
);

nerikiri::Operation decrement( 
       {{"name", "decrement"},
          {"defaultArg", {
            {"arg01", 0}
          }}
        },
		   [](auto arg) { 
		     arg["arg01"] = arg["arg01"].intValue() - 1;
		     return arg;
		   }
);

nerikiri::Operation add( 
       {{"name", "add"},
          {"defaultArg", {
            {"arg01", 0},
            {"arg02", 0}
          }}
        },
		   [](auto arg) { 
		     return Value({{"result", arg["arg01"].intValue() + arg["arg02"].intValue()}});
		   }
);

nerikiri::Operation zero( 
       {{"name", "zero"},
          {"defaultArg", {}}
        },
		   [](auto arg) { 
		     return Value({{"result", 0}});
		   }
);

int main(const int argc, const char* argv[]) {
  auto broker = nerikiri::http::brokerProxy("localhost", 8080);

  std::cout << nerikiri::str(broker->info()) << std::endl;;
  //std::cout << nerikiri::str(broker->invokeOperationByName("increment")) << std::endl;;
  std::cout << nerikiri::str(broker->makeConnection(
    {{"name", "connection01"},
      {"provider", {
          {"process", {
            {"name", {"one"}}
          }},
          {"broker", {
            {"name", "HTTPBroker"},
            {"address", "localhost"},
            {"port", 8080}
          }}
        }},
      {"consumer", {
          {"process", {
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
