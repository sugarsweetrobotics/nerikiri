#include <juiz/nerikiri.h>
#include <juiz/process.h>


int main(const int argc, const char* argv[]) {
  return nerikiri::process(argc, argv)->start();
}
