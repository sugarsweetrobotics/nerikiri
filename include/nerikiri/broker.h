/***
 * base64.h
 * @author Yuki Suga
 * @copyright SUGAR SWEET ROBOTICS, 2020
 * @brief Brokerのインターフェース定義
 */

#pragma once

#include <memory>

#include "nerikiri/logger.h"
#include "nerikiri/value.h"
#include "nerikiri/object.h"


namespace nerikiri {

  class Process;

  /**
   * Brokerの基本クラス
   */
  class Broker : public Object {
  private:
  public:

  public:
    Broker() : Object() {}
    Broker(const Value& info) : Object(info) {}
    virtual ~Broker() {}

    virtual bool run(Process* process) {
      logger::trace("Broker::run()");
      info_["state"] = "running";
      return true;
    }
    
    virtual void shutdown(Process* process) {
      logger::trace("Broker::shutdown()");
      info_["state"] = "stopped";
    }

    bool isRunning() const { return info_.at("state").stringValue() == "running"; }

    // static std::shared_ptr<Broker> null;
  };

  inline std::shared_ptr<Broker> nullBroker() {
    return std::make_shared<Broker>();
  }
}
