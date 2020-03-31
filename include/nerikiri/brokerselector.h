#pragma once

#include <vector>
#include <map>
#include <thread>
#include <future>
#include "nerikiri/broker.h"
#include "nerikiri/functional.h"



namespace nerikiri {

    class BrokerSelector {
    public:
      std::vector<std::shared_ptr<nerikiri::BrokerFactory>> brokerFactories_;

    public:
      BrokerSelector(); 
      virtual ~BrokerSelector();


    public:
      BrokerSelector& addFactory(std::shared_ptr<BrokerFactory> brokerFactory) {
          brokerFactories_.push_back(brokerFactory);
      }

      std::shared_ptr<BrokerAPI> create(const Value& value);


    };
};
