#pragma once

#include "nerikiri/operation_api.h"

namespace nerikiri {

  class OperationInletBase;
  class OperationOutletBase;

  class OperationBase : public OperationAPI {
  protected:
    std::shared_ptr<OperationInletBase> event_inlet_; //< 引数に影響なし
    std::shared_ptr<OperationInletBase> argument_inlet_; //< 全引数に影響する接続につかうinlet
    std::vector<std::shared_ptr<OperationInletBase>> inlets_;
    std::shared_ptr<OperationOutletBase> outlet_;
  public:

    virtual Value fullInfo() const override;

    virtual std::shared_ptr<OutletAPI> outlet() const override;

    virtual std::shared_ptr<InletAPI> inlet(const std::string& name) const override;
    
    virtual std::vector<std::shared_ptr<InletAPI>> inlets() const override;
      
  public:
    OperationBase(const std::string& className, const std::string& typeName, const std::string& fullName, const Value& defaultArg = {});

    virtual ~OperationBase();

  public:
    // callはこのクラスの継承先クラスで実装する
    // virtual Value call() override { return {}; } 

    virtual Value invoke() override;

	  virtual Value execute() override;

    virtual Value info() const override;
  };
}
