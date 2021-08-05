#include <juiz/juiz.h>
#include <juiz/process.h>


int main(const int argc, const char* argv[]) {
  return juiz::process(argc, argv)->start();
}
