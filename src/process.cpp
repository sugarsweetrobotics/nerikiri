#include "nerikiri/process.h"

using namespace nerikiri;

Process::Process() {}

Process::~Process() {}

Process& Process::setOperation(Operation&& op) {
  return *this;
}

Process& Process::setBroker(Broker&& brk) {
  return *this;
}

int32_t Process::start() {
  return -1;
}

