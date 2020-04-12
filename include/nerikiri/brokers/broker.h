#pragma once

#include <memory>
//#include "nerikiri/brokerinfo.h"
#include "nerikiri/logger.h"
#include "nerikiri/value.h"
#include "nerikiri/object.h"


namespace nerikiri {

  class Process;


  class Broker : public Object{
  private:
  public:

  public:
    Broker(const Value& info) : Object(info) {}
    virtual ~Broker() {}

    virtual bool run(Process* process) {
      logger::trace("Broker::run()");
      return false;
    }
    
    virtual void shutdown(Process* process) {
      logger::trace("Broker::shutdown()");
    }

  };


  //static std::shared_ptr<Broker> null;

}
