#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/http/httpbroker.h"
#include "nerikiri/systemeditor.h"
#include "nerikiri/container.h"
#include "nerikiri/containeroperation.h"
#include "nerikiri/containeroperationfactory.h"
#include "nerikiri/ec.h"

using namespace std::literals::string_literals;

using namespace nerikiri;

#include "MyStruct.h"

using MyContainer  = ContainerFactory<MyStruct>;

auto myContainer = std::make_shared<MyContainer>();// myContainer;

auto f = std::shared_ptr<nerikiri::ExecutionContextFactory>(new TimerECFactory());

int main(const int argc, const char* argv[]) {
  std::cout << "MyContainer:" << typeid(MyContainer).name() << std::endl;
  return nerikiri::Process(argv[0])
    .addExecutionContextFactory(f)
    .addBrokerFactory(std::shared_ptr<BrokerFactory>(new nerikiri::http::HTTPBrokerFactory()))
    .addSystemEditor(nerikiri::systemEditor("system_editor", 8080, 8000, 8002))
    .setOnStarting([](auto process) {
      auto oneinfo = process->loadOperationFactory({{"name", "one"}});
      auto addinfo = process->loadOperationFactory({{"name", "add"}});
      auto incinfo = process->loadOperationFactory({{"name", "increment"}});

      auto cfinfo = process->loadContainerFactory({{"name", "MyStruct"}});
      auto addIntinfo = process->loadContainerOperationFactory({{"container_name", "MyStruct"}, {"name", "addInt"}});
      auto intGetterinfo = process->loadContainerOperationFactory({{"container_name", "MyStruct"}, {"name", "intGetter"}});
      
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
