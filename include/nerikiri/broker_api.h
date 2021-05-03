/***
 * broker_api.h
 * @author Yuki Suga
 * @copyright SUGAR SWEET ROBOTICS, 2020
 * @brief Brokerのインターフェース定義
 */

#pragma once

#include <memory>

#include "nerikiri/logger.h"
#include "nerikiri/value.h"
#include "nerikiri/object.h"

#include <nerikiri/process_api.h>

namespace nerikiri {
    
    class ProcessAPI;
    
    /**
     * Brokerの基本クラス
     */
    class BrokerAPI : public Object {
    private:
    public:

    public:
        BrokerAPI(const std::string& className, const std::string& typeName, const std::string& fullName): Object(className, typeName, fullName) {}
        
        virtual ~BrokerAPI() {}        
        virtual Value fullInfo() const = 0;


        virtual bool run(const std::shared_ptr<BrokerProxyAPI>& coreBroker) = 0;
        
        virtual void shutdown(const std::shared_ptr<BrokerProxyAPI>& coreBroker) = 0;

        virtual bool isRunning() const = 0;
    };


    std::shared_ptr<BrokerAPI> nullBroker();

}
