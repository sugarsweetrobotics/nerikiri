#include "nerikiri/nerikiri.h"
#include "nerikiri/http/httpbroker.h"
#include "nerikiri/systemeditor.h"
using namespace std::literals::string_literals;

using namespace nerikiri;

auto increment = nerikiri::OperationFactory( 
      {
        {"name", "increment"},
        {"defaultArg", {{"arg01", 0}}}
      },
		   [](auto arg) { 
		     arg["arg01"] = arg["arg01"].intValue() + 1;
		     return arg;
		   });

int main(const int argc, const char* argv[]) {
  return nerikiri::Process(argv[0])
    .addOperation(increment.create())
    .addBroker(nerikiri::http::broker("localhost", 8080))
    .addSystemEditor(nerikiri::systemEditor("system_editor", 8080, 8000, 8002))
    .start();
}
