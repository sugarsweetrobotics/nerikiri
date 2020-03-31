#include <iostream>

#include "nerikiri/nerikiri.h"
#include "nerikiri/http/httpbroker.h"
#include "nerikiri/systemeditor.h"

#include "nerikiri/brokerselector.h"

using namespace std::literals::string_literals;

using namespace nerikiri;


nerikiri::Value connect(const std::string& connectionName, Broker_ptr providerBroker, const std::string& providerName, Broker_ptr consumerBroker, const std::string& consumerName, const std::string& consumerArgName) {
  // ここは無駄が多い．providerのNameとBrokerを同時に物OperationProxyなどのクラスを定義するべきか，
  // OperationInfoもショートバージョンなどを作れば軽量にできるかも．今のところnameしか使わない．defaultArgsは無駄負荷だし，
  // 今後はtestSetが入るから・・・
  return providerBroker->makeConnection(
    { {"name", connectionName},
      {"output", {
          {"info", providerBroker->getOperationInfo({{"name", providerName}})},
          {"broker", providerBroker->getBrokerInfo()}}
      },
      {"input", {
          {"info", consumerBroker->getOperationInfo({{"name", consumerName}})},
          {"broker", consumerBroker->getBrokerInfo()},
          {"target", {
              {"name", consumerArgName}}
          }}}
    } );
}

int main(const int argc, const char* argv[]) {
  auto s = nerikiri::BrokerSelector();
  s.addFactory(std::make_shared<nerikiri::http::HTTPBrokerFactory>());
  auto broker = s.create({{"name", "HTTPBroker"}, {"host","localhost"}, {"port", 8080}});
  //auto broker = nerikiri::http::brokerProxy("localhost", 8080);
  std::cout << nerikiri::str(broker->getBrokerInfo()) << std::endl;;
  
  
  std::cout << nerikiri::str(connect("connection01", 
          broker,
          "one", 
          broker,
          "increment", 
          "arg01")) << std::endl;

  std::cout << nerikiri::str(broker->invokeOperationByName("increment")) << std::endl;;
  return 0;
}
