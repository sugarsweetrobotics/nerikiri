/***
 * broker_api.h
 * @author Yuki Suga
 * @copyright SUGAR SWEET ROBOTICS, 2020
 * @brief Brokerのインターフェース定義
 */

#pragma once

#include <memory>
#include <string>

#include "nerikiri/object.h"
#include <nerikiri/process_api.h>

namespace nerikiri {
    
    /**
     * Brokerの基本クラス
     */
    class BrokerAPI : public Object {
    private:
    public:

    public:
        BrokerAPI(const std::string& className, const std::string& typeName, const std::string& fullName): Object(className, typeName, fullName) {}
        
        virtual ~BrokerAPI() {}

        virtual bool run(const std::shared_ptr<BrokerProxyAPI>& coreBroker) = 0;
        
        virtual void shutdown(const std::shared_ptr<BrokerProxyAPI>& coreBroker) = 0;

        virtual bool isRunning() const = 0;
    };


    std::shared_ptr<BrokerAPI> nullBroker();



    class BrokerFactoryAPI : public Object {
    private:
    public:
        BrokerFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}
        virtual ~BrokerFactoryAPI() {}
    public:
        virtual std::shared_ptr<BrokerAPI> create(const Value& param) = 0;

        virtual std::shared_ptr<BrokerProxyAPI> createProxy(const Value& param) = 0;
    };

    std::shared_ptr<BrokerFactoryAPI> nullBrokerFactory();
}
