#pragma once
#include <memory>
#include <string>
#include <condition_variable>
#include "nerikiri/runnable.h"


namespace nerikiri {


  class SystemEditor : public Runnable {

  public:
    virtual ~SystemEditor() {}

  public:
    virtual bool run() override { return false; };

    virtual bool shutdown() override { return false; };

  public:

    std::string name() { return name_; }
  private:
    const std::string name_;    
  };



  using SystemEditor_ptr = std::unique_ptr<SystemEditor>;

  SystemEditor_ptr systemEditor(const std::string& name, const int32_t api_port, const int32_t port=8000, const int32_t websocket_port=8002);
};
