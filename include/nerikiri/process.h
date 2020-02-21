#pragma once

#include "operation.h"
#include "broker.h"
#include "brokerdictionary.h"
#include "nerikiri/runnable.h"
#include "nerikiri/systemeditor.h"


namespace nerikiri {

  class ProcessInfo {
  public:
    std::string name;

    ProcessInfo(const std::string& n): name(n) {}
  };
  
  class Process {
  private:
    BrokerDictionary brokerDictionary_;
    std::map<std::string, SystemEditor_ptr> systemEditors_;
    std::vector<std::thread> threads_;
    const ProcessInfo info_;
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
    
    std::vector<OperationInfo> getOperationInfos();
  };


  //  inline Process process(std::vector<std::string> args) { return std::move(Process(args.size() > 0 ? args[0] : "default_process_name")); }

}
