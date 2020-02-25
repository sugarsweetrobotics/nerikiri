#include "nerikiri/nerikiri.h"
#include "nerikiri/http/httpbroker.h"
#include "nerikiri/systemeditor.h"

int main(const int argc, const char* argv[]) {
  return nerikiri::Process(argv[0])
    .addOperation({{"increment"},
		   [](auto arg) { 
		     arg["arg01"] = arg["arg01"].intValue() + 1;
		     return arg;
		   }})
    .addBroker(nerikiri::http::broker("localhost", 8080))
    .addSystemEditor(nerikiri::systemEditor())
    .start();
}
