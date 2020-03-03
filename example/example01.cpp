#include "nerikiri/nerikiri.h"
#include "nerikiri/http/httpbroker.h"
#include "nerikiri/systemeditor.h"

nerikiri::Value v(0);
nerikiri::Value v2("hoeg");

nerikiri::Value defaultArg({
  //{"name", {"increment"}},
  {"arg01", v}
});

nerikiri::OperationInfo info({
  {"name", {"increment"}},
  {"defaultArg", defaultArg }
});


int main(const int argc, const char* argv[]) {
  return nerikiri::Process(argv[0])
    .addOperation(nerikiri::Operation( 
       info,
		   [](auto arg) { 
		     arg["arg01"] = arg["arg01"].intValue() + 1;
		     return arg;
		   }))
    .addBroker(nerikiri::http::broker("localhost", 8080))
    .addSystemEditor(nerikiri::systemEditor("system_editor", 8080, 8000, 8002))
    .start();
}
