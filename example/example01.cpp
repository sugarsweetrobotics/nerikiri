#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"
//#include "nerikiri/http/httpbroker.h"
#include "nerikiri/systemeditor.h"
#include "nerikiri/containers/container.h"
#include "nerikiri/containers/containeroperation.h"
#include "nerikiri/containers/containeroperationfactory.h"
#include "nerikiri/ec.h"

using namespace std::literals::string_literals;

using namespace juiz;

int main(const int argc, const char* argv[]) {
  return juiz::Process(argc, argv)
    //.addExecutionContextFactory(f)
    //.addBrokerFactory(std::shared_ptr<BrokerFactory>(new juiz::http::HTTPBrokerFactory()))
    .addSystemEditor(juiz::systemEditor("system_editor", 8080, 8000, 8002))
    .setOnStarting([](auto process) {
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
