#include "nerikiri/nerikiri.h"
#include "nerikiri/http/httpbroker.h"
#include "nerikiri/systemeditor.h"
using namespace std::literals::string_literals;

using namespace nerikiri;

nerikiri::OperationFactory increment( 
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

nerikiri::OperationFactory decrement( 
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

nerikiri::OperationFactory add( 
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

nerikiri::OperationFactory zero( 
       {{"name", "zero"},
          {"defaultArg", {}}
        },
		   [](auto arg) { 
		     return Value({0});
		   }
);


nerikiri::OperationFactory one( 
       {{"name", "one"},
          {"defaultArg", {}}
        },
		   [](auto arg) { 
		     return Value({1});
		   }
);


int main(const int argc, const char* argv[]) {
  return nerikiri::Process(argv[0])
    .addOperation(increment.create())
    .addOperation(decrement.create())
    .addOperation(add.create())
    .addOperation(zero.create())
    .addOperation(one.create())
    .addBroker(nerikiri::http::broker("0.0.0.0", 8080))
    .addSystemEditor(nerikiri::systemEditor("system_editor", 8080, 8000, 8002))
    .start();
}
