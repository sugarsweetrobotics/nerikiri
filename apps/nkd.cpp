#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"


int main(const int argc, const char* argv[]) {
  return nerikiri::process(argc, argv)->start();
}
