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
    class Broker : public Object {
    private:
    public:

    public:
        Broker();
        Broker(const Value& info);
        virtual ~Broker();

        virtual bool run(Process* process);
        
        virtual void shutdown(Process* process);

        bool isRunning() const;
    };

    std::shared_ptr<Broker> nullBroker();
}
