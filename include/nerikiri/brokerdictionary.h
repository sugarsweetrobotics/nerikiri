#pragma once

#include <vector>
#include <map>
#include <thread>
#include <future>
#include "nerikiri/broker.h"
#include "nerikiri/functional.h"



namespace nerikiri {

  /*
  template<typename T, typename A>
  std::vector<T> map(std::vector<A>& aa, std::function<T(A&)> func) {
    std::vector<T> t;
    for(auto&&a : aa) {
      t.emplace_back(func(a));
    }
    return t;
  }
  */

  class BrokerDictionary {
  private:
    std::map<std::string, Broker_ptr> dictionary_;

  public:
    void add(Broker_ptr&& broker);

    void foreach(std::function<void(Broker_ptr&)> func);
    
    template<typename T>
    std::vector<T> map(std::function<T(Broker_ptr&)> func) {
      logger::trace("BrokerDictionary::map()");
      std::vector<T> ts;
      for(auto& [name, broker] : dictionary_) {
	logger::trace(" - for broker {}", str(broker->info()));
	ts.emplace_back(func(broker));
      }
      logger::trace("BrokerDictionary::map ends");
      return ts;
    }
  };

};
