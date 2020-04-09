#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/http/httpbroker.h"
#include "nerikiri/systemeditor.h"
#include "nerikiri/container.h"
#include "nerikiri/ec.h"

using namespace std::literals::string_literals;

using namespace nerikiri;

auto increment = std::shared_ptr<nerikiri::OperationFactory>(new nerikiri::OperationFactory(
       {{"name", "increment"},
          {"defaultArg", {
            {"arg01", 0}
          }}
        },
		   [](auto arg) { 
         std::cout << "Increment is called." << std::endl;
		     return Value(arg["arg01"].intValue() + 1);
		   }
));

auto decrement = std::shared_ptr<nerikiri::OperationFactory>(new nerikiri::OperationFactory(
       {{"name", "decrement"},
          {"defaultArg", {
            {"arg01", 0}
          }}
        },
		   [](auto arg) { 
		     return Value(arg["arg01"].intValue() - 1);
		   }
));

auto add = std::shared_ptr<nerikiri::OperationFactory>(new nerikiri::OperationFactory(
       {{"name", "add"},
          {"defaultArg", {
            {"arg01", 0},
            {"arg02", 0}
          }}
        },
		   [](auto arg) { 
		     return Value({{"result", arg["arg01"].intValue() + arg["arg02"].intValue()}});
		   }
));


auto zero = std::shared_ptr<nerikiri::OperationFactory>(new nerikiri::OperationFactory(
       {{"name", "zero"},
          {"defaultArg", {}}
        },
		   [](auto arg) { 
		     return Value({0});
		   }
));


auto one = std::shared_ptr<nerikiri::OperationFactory>(new nerikiri::OperationFactory(
       {{"name", "one"},
          {"defaultArg", {}}
        },
		   [](auto arg) { 

         std::cout << "One is called." << std::endl;
		     return Value({1});
		   }
));

struct MyStruct {
public:
  int64_t intValue;
  double  doubleValue;
};


using MyContainer  = ContainerFactory<MyStruct>;
using MyContainerOperation = ContainerOperationFactory<MyStruct>;

auto myContainer = std::make_shared<MyContainer>();// myContainer;

auto intGetter = std::shared_ptr<ContainerOperationFactoryBase>(new MyContainerOperation(
  
    {
      {"name", "intGetter"},
      {"defaultArg", {}},
    },
    [](auto& container, auto arg) {
      return Value(container.intValue);
    }
));

auto addInt = std::shared_ptr<ContainerOperationFactoryBase>(new MyContainerOperation(
    {
      {"name", "addInt"},
      {"defaultArg", {{"data", 1}}}
    },
    [](auto& container, auto arg) {
      container.intValue += arg.at("data").intValue();
      return Value(container.intValue);
    }
));

auto f = std::shared_ptr<nerikiri::ExecutionContextFactory>(new TimerECFactory());

int main(const int argc, const char* argv[]) {
  std::cout << "MyContainer:" << typeid(MyContainer).name() << std::endl;
  return nerikiri::Process(argv[0])
    .addOperationFactory(increment)
    .addOperationFactory(decrement)
    .addOperationFactory(add)
    .addOperationFactory(zero)
    .addOperationFactory(one)
    .addExecutionContextFactory(f)
    .addContainerFactory(std::static_pointer_cast<ContainerFactoryBase>(myContainer))
    .addContainerOperationFactory(intGetter)
    .addContainerOperationFactory(addInt)
    .addBrokerFactory(std::shared_ptr<BrokerFactory>(new nerikiri::http::HTTPBrokerFactory()))
    .addSystemEditor(nerikiri::systemEditor("system_editor", 8080, 8000, 8002))
    .setOnStarting([](auto process) {
      auto ecInfo = process->createExecutionContext({{"name", "TimerEC"}, {"rate", 1.0}});
      auto binfo = process->createBroker({{"name", "HTTPBroker"}, {"host", "0.0.0.0"}, {"port", 8080}});
      auto opInfo = process->createOperation({ {"name", "one"} });
      auto opInfo2 = process->createOperation({ {"name", "increment"} });
      auto c = process->createContainer({{"name", demangle(typeid(MyStruct).name())}});
      process->createContainerOperation(c, {{"name", "intGetter"}});
      process->createContainerOperation(c, {{"name", "addInt"}});
      process->bindECtoOperation(ecInfo.at("instanceName").stringValue(), opInfo);
      process->store()->getExecutionContext(ecInfo)->start();
    })
    .setOnStarted([](auto process) {
      auto b = process->createBrokerProxy({{"name", "HTTPBroker"}, {"host", "localhost"}, {"port", 8080}});
      
      b->registerProviderConnection( { {"name","connection01"}, {"type","event"}, 
        {"output", {
            {"info", b->getOperationInfo({{"instanceName", "one0.ope"}})},
            {"broker", b->getBrokerInfo()}}
        },
        {"input", {
            {"info", b->getOperationInfo({{"instanceName", "increment0.ope"}})},
            {"broker", b->getBrokerInfo()},
            {"target", {
                {"name", "arg01"}}
            }}}
      } );
    })
    .start();
}
