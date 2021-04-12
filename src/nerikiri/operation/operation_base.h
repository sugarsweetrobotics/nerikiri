#pragma once

//#include <map>
//#include <functional>
//#include <thread>
//#include <mutex>

//#include "nerikiri/nerikiri.h"
//#include "nerikiri/object.h"
#include "nerikiri/operation_api.h"

//#include "nerikiri/connection.h"
//#include "nerikiri/functional.h"

//#include "newest_value_buffer.h"

//#include "operation_inlet_base.h"

namespace nerikiri {

  //class Process;

  //using Process_ptr = Process*;


  class OperationInletBase;
  class OperationOutletBase;

  class OperationBase : public OperationAPI {
  protected:
    //const std::string& operationTypeName_;
    std::vector<std::shared_ptr<OperationInletBase>> inlets_;
    std::shared_ptr<OperationOutletBase> outlet_;
    std::shared_ptr<OperationInletBase> event_inlet_;
  public:

    virtual Value fullInfo() const override;

    virtual std::shared_ptr<OperationOutletAPI> outlet() const override;

    virtual std::shared_ptr<OperationInletAPI> inlet(const std::string& name) const override;
    
    virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override;
      
  public:
    OperationBase(const std::string& className, const std::string& typeName, const std::string& fullName, const Value& defaultArg = {});

    // OperationBase(const OperationBase& op);

    virtual ~OperationBase();

  public:

    // virtual Value call() override { return {}; } 

    virtual Value invoke() override;

	  virtual Value execute() override;

    virtual Value info() const override;
  };
}
