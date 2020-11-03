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

namespace nerikiri {
    

    class Process;

    /**
     * Brokerの基本クラス
     */
    class BrokerAPI : public Object {
    private:
    public:

    public:
        BrokerAPI(const std::string& typeName, const std::string& fullName): Object(typeName, fullName) {}
        virtual ~BrokerAPI() {}

        virtual bool run(Process* process) = 0;
        
        virtual void shutdown(Process* process) = 0;

        virtual bool isRunning() const = 0;
    };

    class NullBroker : public BrokerAPI {
    private:
    public:

    public:
        NullBroker(): BrokerAPI("NullBroker", "null") {}
        virtual ~NullBroker() {}

        virtual bool run(Process* process) override {
            logger::error("NullBroker::{}() failed. Object is null.", __func__);
            return false;
        }
        
        virtual void shutdown(Process* process) override {
            logger::error("NullBroker::{}() failed. Object is null.", __func__);
            return;
        }

        virtual bool isRunning() const override {
            logger::error("NullBroker::{}() failed. Object is null.", __func__);
            return false;
        }
    };

}
