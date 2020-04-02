#include <sstream>
#include "nerikiri/nerikiri.h"
#include "nerikiri/functional.h"
#include "nerikiri/process.h"
#include "nerikiri/logger.h"
#include "nerikiri/signal.h"


#include <iostream>

using namespace nerikiri;
using namespace nerikiri::logger;


Process Process::null("");

Process::Process(const std::string& name) : info_({{"name", name}}), store_(this) {
  logger::trace("Process::Process(\"{}\")", name);
  if (name.length() == 0) { 
    info_ = Value::error("Process is Null."); 
  } 
  setLogLevel(TRACE);
}

Process::~Process() {
  logger::trace("Process::~Process()");
}


Process& Process::addSystemEditor(SystemEditor_ptr&& se) {
  logger::trace("Process::addSystemEditor()");
  systemEditors_.emplace(se->name(), std::forward<SystemEditor_ptr>(se));
  return *this;
}

Process& Process::addBroker(const Broker_ptr& brk) {
  logger::trace("Process::addBroker()");
  if (brk) {
    brk->setProcess(Process_ptr(this));
    brk->setProcessStore(&store_);
    brokers_.push_back(brk);
    //brokers_.emplace_back(std::forward<Broker_ptr>(brk));
  } else {
    logger::error("addBroker failed. Added broker is null.");
  }
  return *this;
}

static auto start_broker(std::vector<std::thread>& threads, Process* process, Broker_ptr& brk) {
  logger::trace("Creating a thread for broker {}", str(brk->getBrokerInfo()));
  std::promise<bool> prms;
  auto ftr = prms.get_future();
  threads.emplace_back(std::thread([&brk, process](std::promise<bool> p) {
				     std::stringstream ss;
				     ss << std::this_thread::get_id();
				     logger::trace("A thread {} is going to run broker {}", ss.str(), str(brk->getBrokerInfo()));
				     p.set_value(brk->run(process));
				     logger::trace("A thread {} finished", ss.str());
				   }, std::move(prms)));
  return ftr;
}

static std::future<bool> start_systemEditor(std::vector<std::thread>& threads, const SystemEditor_ptr& se) {
  logger::trace("Creating a thread for SystemEditor {}", se->name());
  std::promise<bool> prms;
  auto ftr = prms.get_future();
  threads.emplace_back(std::thread([&se](std::promise<bool> p) {
				     std::stringstream ss;
				     ss << std::this_thread::get_id();
				     logger::trace("A thread {} is going to run SystemEditor {}", ss.str(), se->name());
				     p.set_value(se->run());
				     logger::trace("A thread {} finished", ss.str());
				   }, std::move(prms)));
  return ftr;
}

void Process::startAsync() {
  logger::trace("Process::startAsync()");
  auto broker_futures = nerikiri::map<std::future<bool>, Broker_ptr>(brokers_, [this](auto& brk) -> std::future<bool> {
								   return start_broker(this->threads_, this, brk);
								 });
  
  auto systemEditor_futures = nerikiri::map<std::future<bool>, std::string, SystemEditor_ptr>(this->systemEditors_,
											      [this](const auto& name, const auto& se) {
												return start_systemEditor(this->threads_, se); }
											      );

}
  
int32_t Process::wait() {
  logger::trace("Process::wait()");
  if (!wait_for(SIGNAL_INT)) {
    logger::error("Process::start method try to wait for SIGNAL_INT, but can not.");
    return -1;
  }
  return 0;
}

void Process::shutdown() {
  logger::trace("Process::shutdown()");
  nerikiri::foreach<Broker_ptr>(brokers_, [this](auto& brk) {
			      brk->shutdown(this);
			    });
  nerikiri::foreach<std::string, SystemEditor_ptr>(systemEditors_, [this](auto& name, auto& se) {
				      se->shutdown();
				    });
  
  logger::trace(" - joining...");
  for(auto& t : this->threads_) {
    t.join();
  }
  logger::trace(" - joined");
}


int32_t Process::start() {
  logger::trace("Process::start()");
  startAsync();
  if (wait() < 0) return -1;
  shutdown();
  return 0;
}

Broker_ptr Process::getBrokerByName(const std::string& name) {
  for(auto& brk : brokers_) {
    if (brk->getBrokerInfo().at("name").stringValue() == name) {
      return brk;
    }
  }
  return Broker::null;
}

Broker_ptr Process::createBrokerProxy(const BrokerInfo& bi) {
  logger::trace("Process::createBroker({})", str(bi));
  for(auto f : brokerFactories_) {
    if(f->typeName() == bi.at("name").stringValue()) {
      logger::info("Creating Broker ({})", str(bi));
      return f->createProxy(bi);
    }
  }
  logger::error("createBroker failed. Can not found appropreate broker factory.");
  return nullptr;
}

void Process::executeOperation(const OperationInfo& info) const {
  logger::trace("Process::executeOpration({})", str(info));
  
}

Process& Process::addExecutionContext(const ExecutionContext& ec) {
  
  return *this;
}

Process& Process::createBroker(const BrokerInfo& ci) {
  logger::trace("Process::createBroker({})", str(ci));
  for(auto f : brokerFactories_) {
    if(f->typeName() == ci.at("name").stringValue()) {
      logger::info("Creating Broker ({})", str(ci));
      return addBroker(f->create(ci));
    }
  }
  logger::error("createBroker failed. Can not found appropreate broker factory.");
  return *this;
}
    

/**
 * もしConnectionInfoで指定されたBrokerが引数のbrokerでなければ，親プロセスに対して別のブローカーをリクエストする
 */
Value checkProcessHasProviderOperation(Process* process, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;    
    if (process->store()->getOperation(ci.at("output").at("info")).isNull()) {
      return Value::error(logger::warn("The broker received makeConnection does not have the provider operation."));
    }
    return ci;
}

Value Process::makeConnection(const ConnectionInfo& ci, BrokerAPI* receiverBroker) {
  logger::trace("Process::makeConnection({})", str(ci));
  auto ret = ci;
  // 自分プロセスがProvider側Operatinを持っていなければ，provider側のブローカープロキシを作ってそちらを呼び出す．
  if (checkProcessHasProviderOperation(this, ci).isError()) {
    auto providerBroker = createBrokerProxy(ci.at("output").at("broker"));
    if (!providerBroker) {
      return Value::error(logger::error("makeConnection failed. Provider side broker can not be created. {}", str(ci.at("output"))));
    }
  }

  if (store()->getOperation(ci.at("output").at("info")).hasOutputConnectionRoute(ci)) {
      return Value::error(logger::warn("makeConnection failed. Provider already have the same connection route {}", str(ci.at("output"))));
  }

  while (store()->getOperation(ret.at("output").at("info")).hasOutputConnectionName(ret)) {
      logger::warn("makeConnection failed. Provider already have the same connection route {}", str(ret.at("output")));
      ret["name"] = ci.at("name").stringValue() + "_2";
  }

  auto consumerBroker = createBrokerProxy(ret.at("input").at("broker"));
  if (!consumerBroker) {
    return Value::error(logger::error("makeConnection failed. Consumer side broker can not be created. {}", str(ci.at("output"))));
  }
  ret = consumerBroker->registerConsumerConnection(ci);

  auto providerBroker = createBrokerProxy(ci.at("output").at("broker"));
  if (!providerBroker) {
    return Value::error(logger::error("makeConnection failed. Provider side broker can not be created. {}", str(ci.at("output"))));
  }
  return providerBroker->registerProviderConnection(ret);
}

Value Process::registerConsumerConnection(const ConnectionInfo& ci) {
    logger::trace("Process::registerConsumerConnection({}", str(ci));
    if (ci.isError()) return ci;
    auto ret = ci;
    /// Consumer側でなければ失敗出力
    auto& consumer = store()->getOperation(ci.at("input").at("info"));
    if (consumer.isNull()) {
        return Value::error(logger::error("registerConsumerConnection failed. The broker does not have the consumer ", str(ci.at("input"))));
    }

    if (consumer.hasInputConnectionRoute(ci)) {
        return Value::error(logger::error("registerConsumerConnection failed. Consumer already have the same connection.", str(ci.at("input"))));
    }

    while (consumer.hasInputConnectionName(ret)) {
        logger::warn("registerConsumerConnection failed. Consumer already have the same connection.", str(ci.at("input")));
        ret["name"] = ret["name"].stringValue() + "_2";
    }

    auto brokerProxy = createBrokerProxy(ci.at("output").at("broker"));
    if (!brokerProxy) {
      return Value::error(logger::error("registerConsumerConnection failed. Can not create Provider side broker in consumer process. {}", str(ci.at("output").at("broker"))));
    }
    ret = consumer.addConsumerConnection(consumerConnection(ret, brokerProxy));
    if (ret.isError()) {
      // 登録が失敗ならConsumer側のConnectionを破棄。
      auto ret = nerikiri::removeConsumerConnection(createBrokerProxy(ci.at("output").at("broker")), ci);
      return Value::error(logger::error("request registerConsumerConnection for consumer's broker failed. ", ret.getErrorMessage()));
    }
    return ret;
  }


Value Process::registerProviderConnection(const ConnectionInfo& ci) {
    logger::trace("Process::registerProviderConnection({}", str(ci));
    if (ci.isError()) return ci;
    // リクエストが成功なら、こちらもConnectionを登録。
    auto& provider = store()->getOperation(ci.at("output").at("info"));
    auto brokerProxy = createBrokerProxy(ci.at("input").at("broker"));
    if (!brokerProxy) {
      return Value::error(logger::error("registerProviderConnection failed. Can not create Consumer side broker in provider process. {}", str(ci.at("output").at("broker"))));
    }
    auto ret = provider.addProviderConnection(providerConnection(ci, brokerProxy));
    if (ret.isError()) {
        // 登録が失敗ならConsumer側のConnectionを破棄。
        auto ret = nerikiri::removeConsumerConnection(createBrokerProxy(ci.at("input").at("broker")), ci);
        return Value::error(logger::error("request registerProviderConnection for provider's broker failed. ", ret.getErrorMessage()));
    }// 登録成功ならciを返す
    return ret;
}