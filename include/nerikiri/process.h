#pragma once

#include "nerikiri/operation.h"
#include "nerikiri/value.h"
#include "nerikiri/broker.h"
#include "nerikiri/brokerdictionary.h"
#include "nerikiri/connection.h"
#include "nerikiri/connectiondictionary.h"

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
    //BrokerDictionary brokerDictionary_;
    ConnectionDictionary connectionDictionary_;
    std::map<std::string, SystemEditor_ptr> systemEditors_;
    std::vector<std::thread> threads_;
    ProcessInfo info_;
    std::vector<Operation> operations_;
    std::vector<Broker_ptr> brokers_;
  public:
    Process(const std::string& name);
    ~Process();


  public:
    Process& addOperation(Operation&& op);
    Process& addOperation(const Operation& op);
    Operation& getOperationByName(const std::string& name);
    
    Process& addBroker(Broker_ptr&& brk);
    Broker_ptr& getBrokerByName(const std::string& name);

    Broker_ptr& getBrokerByBrokerInfo(const BrokerInfo& ci);
    
    Process& addSystemEditor(SystemEditor_ptr&& se);
    Process& addConnection(Connection_ptr&& con);

    int32_t start();
    void startAsync();
    int32_t wait();
    void shutdown();
    ProcessInfo info() const { return info_; }
    
    OperationInfos getOperationInfos();

    public:


    Value invokeOperationByName(const std::string& name);
    Value invokeConnection(const Connection& con);
  };


  //  inline Process process(std::vector<std::string> args) { return std::move(Process(args.size() > 0 ? args[0] : "default_process_name")); }

}
