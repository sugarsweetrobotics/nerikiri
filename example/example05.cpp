#include "nerikiri/nerikiri.h"
#include "nerikiri/http/httpbroker.h"
#include "nerikiri/systemeditor.h"
#include "nerikiri/container.h"

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
		     return Value({0});
		   }
);


nerikiri::Operation one( 
       {{"name", "one"},
          {"defaultArg", {}}
        },
		   [](auto arg) { 
		     return Value({1});
		   }
);

struct MyStruct {
public:
  int32_t intValue;
  double  doubleValue;
};


using MyContainer  = Container<MyStruct>;
using MyContainerOperation = ContainerOperation<MyStruct>;

MyContainerOperation intGetter(
    {
      {"name", "intGetter"},
      {"defaultArg", {}},
    },
    [](auto& container, auto arg) {
      return Value((int64_t)container->intValue);
    }
);

MyContainerOperation addInt(
    {
      {"name", "addInt"},
      {"defaultArg", {{"data", 1}}}
    },
    [](auto& container, auto arg) {
      container->intValue += arg.at("data").intValue();
      return Value((int64_t)container->intValue);
    }
);

int main(const int argc, const char* argv[]) {
  return nerikiri::Process(argv[0])
    .addOperation(increment)
    .addOperation(decrement)
    .addOperation(add)
    .addOperation(zero)
    .addOperation(one)
    .addContainer(new MyContainer({{"name", "MyContainer"}}))
    .addOperationToContainerByName("MyContainer", &intGetter)
    .addOperationToContainerByName("MyContainer", &addInt)
    .addBroker(nerikiri::http::broker("0.0.0.0", 8080))
    .addSystemEditor(nerikiri::systemEditor("system_editor", 8080, 8000, 8002))
    .start();
}
