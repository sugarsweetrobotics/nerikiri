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
        BrokerAPI(const std::string& typeName, const std::string& fullName): Object(typeName, fullName) {}
        
        virtual ~BrokerAPI() {}        
        virtual Value fullInfo() const = 0;


        virtual bool run(ProcessAPI* process) = 0;
        
        virtual void shutdown(ProcessAPI* process) = 0;

        virtual bool isRunning() const = 0;
    };

    class NullBroker : public BrokerAPI {
    private:
    public:

    public:
        NullBroker(): BrokerAPI("NullBroker", "null") {}
        virtual ~NullBroker() {}

        virtual Value fullInfo() const override {
            return Value::error(logger::error("NullBroker::{}() failed. Object is null.", __func__));
        }

        virtual bool run(ProcessAPI* process) override {
            logger::error("NullBroker::{}() failed. Object is null.", __func__);
            return false;
        }
        
        virtual void shutdown(ProcessAPI* process) override {
            logger::error("NullBroker::{}() failed. Object is null.", __func__);
            return;
        }

        virtual bool isRunning() const override {
            logger::error("NullBroker::{}() failed. Object is null.", __func__);
            return false;
        }
    };

}
