#pragma once

#include "operation.h"
#include "broker.h"

namespace nerikiri {

  class Process {


  public:
    Process();
    ~Process();


  public:
    Process& setOperation(Operation&& op);

    Process& setBroker(Broker&& brk);

    int32_t start();
  };
  


}
