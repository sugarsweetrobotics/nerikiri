#pragma once

#include "nerikiri/operation.h"
#include "nerikiri/value.h"
#include "nerikiri/broker.h"
#include "nerikiri/brokerdictionary.h"
#include "nerikiri/runnable.h"
#include "nerikiri/systemeditor.h"


namespace nerikiri {

  /*
  class ProcessInfo : public Value{
  public:
    ProcessInfo(const std::string& n): Value({
      {"name", Value(n)},
      {"type", {"ProcessInfo"}}
      }) {}
    virtual ~ProcessInfo() {}

    const std::string name() const {
      return this->at("name").stringValue();
    }
  };
  */
 using ProcessInfo = Value;
  
  class Process {
  private:
    BrokerDictionary brokerDictionary_;
    std::map<std::string, SystemEditor_ptr> systemEditors_;
    std::vector<std::thread> threads_;
    const ProcessInfo info_;
    std::vector<Operation> operations_;
  public:
    Process(const std::string& name);
    ~Process();


  public:
    Process& addOperation(Operation&& op);
    Process& addBroker(Broker_ptr&& brk);
    Process& addSystemEditor(SystemEditor_ptr&& se);

    int32_t start();
    void startAsync();
    int32_t wait();
    void shutdown();
    const ProcessInfo& info() { return info_; }
    
    OperationInfos getOperationInfos();
  };


  //  inline Process process(std::vector<std::string> args) { return std::move(Process(args.size() > 0 ? args[0] : "default_process_name")); }

}
