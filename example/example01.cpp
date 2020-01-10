#include "nerikiri/nerikiri.h"
#include "nerikiri/http/restbroker.h"


int main(const int argc, const char* argv[]) {
  nerikiri::Process process;
  process.setOperation(nerikiri::Operation([](auto arg) { 
    arg["arg01"] = arg["arg01"].intValue() + 1;
    return arg;
  }));

  process.setBroker(nerikiri::http::RESTBroker());

  return process.start();
}
