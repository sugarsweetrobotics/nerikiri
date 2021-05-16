#pragma once

#include "operation_inlet_base.h"

namespace juiz {


  /**
   * ArgumentInletは，全引数に対して影響を及ぼす接続をするためのInlet
   * このInletは特にFSMで使う．引数指定してon_entryイベントでexecuteしたい場合はこの接続を使う
   * Operation側では__argument__に割り当てられているので，operation_->inlet("__argument__")->＊＊＊()のように使う
   */
  class ArgumentInlet : public OperationInletBase {
  public:
    ArgumentInlet(const std::string& name, OperationAPI* operation, const Value& defaultValue) : OperationInletBase(name, operation, defaultValue)
    {}

    virtual ~ArgumentInlet() {}

    
    virtual Value put(const Value& value) override {  
        logger::trace("ArgumentInlet::put({}) called", value);
        for(const auto& i : operation_->inlets()) {
            if (value.hasKey(i->name())) {
                i->put(value[i->name()]);
            }
        }
        return value;
    }

  };
}