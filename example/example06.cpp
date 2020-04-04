#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/http/httpbroker.h"
#include "nerikiri/systemeditor.h"
#include "nerikiri/container.h"
#include "nerikiri/ec.h"

using namespace std::literals::string_literals;

using namespace nerikiri;

nerikiri::OperationFactory increment( 
       {{"name", "increment"},
          {"defaultArg", {
            {"arg01", 0}
          }}
        },
		   [](auto arg) { 
		     return Value(arg["arg01"].intValue() + 1);
		   }
);

nerikiri::OperationFactory decrement( 
       {{"name", "decrement"},
          {"defaultArg", {
            {"arg01", 0}
          }}
        },
		   [](auto arg) { 
		     return Value(arg["arg01"].intValue() - 1);
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

struct MyStruct {
public:
  int64_t intValue;
  double  doubleValue;
};


using MyContainer  = ContainerFactory<MyStruct>;
using MyContainerOperation = ContainerOperationFactory<MyStruct>;

MyContainer myContainer;

MyContainerOperation intGetter(
    {
      {"name", "intGetter"},
      {"defaultArg", {}},
    },
    [](auto& container, auto arg) {
      return Value(container.intValue);
    }
);

MyContainerOperation addInt(
    {
      {"name", "addInt"},
      {"defaultArg", {{"data", 1}}}
    },
    [](auto& container, auto arg) {
      container.intValue += arg.at("data").intValue();
      return Value(container.intValue);
    }
);

TimerEC timer(1.0); // Hz

int main(const int argc, const char* argv[]) {
  std::cout << "MyContainer:" << typeid(MyContainer).name() << std::endl;
  auto c = myContainer.create();
  c->addOperation(intGetter.create());
  c->addOperation(addInt.create());
  return nerikiri::Process(argv[0])
    .addOperation(increment.create())
    .addOperation(decrement.create())
    .addOperation(add.create())
    .addOperation(zero.create())
    .addOperation(one.create())
    .addExecutionContext(timer)
    .addContainer(c)
    .addBrokerFactory(std::shared_ptr<BrokerFactory>(new nerikiri::http::HTTPBrokerFactory()))
    .createBroker({{"name", "HTTPBroker"}, {"host", "0.0.0.0"}, {"port", 8080}})
    .addSystemEditor(nerikiri::systemEditor("system_editor", 8080, 8000, 8002))
    .start();
}
