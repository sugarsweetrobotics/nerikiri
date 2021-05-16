/***
 * broker_api.h
 * @author Yuki Suga
 * @copyright SUGAR SWEET ROBOTICS, 2020
 * @brief Brokerのインターフェース定義
 */

#pragma once

#include <memory>
#include <string>

#include <juiz/object.h>
#include <juiz/process_api.h>

namespace juiz {
    
    /**
     * Brokerの基本クラス
     */
    class BrokerAPI : public Object {
    private:
    public:

    public:
        BrokerAPI(const std::string& className, const std::string& typeName, const std::string& fullName): Object(className, typeName, fullName) {}
        
        virtual ~BrokerAPI() {}

        /// URLスキーム文字列を返す
        virtual std::string scheme() const = 0;

        /// URLドメイン文字列を返す
        virtual std::string domain() const = 0;

        bool isScheme(const std::string& shm) const {
            return shm == scheme();
        }

        bool isDomain(const std::string& dom) const {
            std::string arg_dom = dom;
            if (arg_dom.at(arg_dom.length()-1) == '/') arg_dom = arg_dom + "/";
            std::string self_dom = domain();
            if (self_dom.at(self_dom.length()-1) == '/') self_dom = self_dom + "/";
            return self_dom == arg_dom;
        }

        virtual bool run(const std::shared_ptr<ClientProxyAPI>& coreBroker) = 0;
        
        virtual void shutdown(const std::shared_ptr<ClientProxyAPI>& coreBroker) = 0;

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

        virtual std::shared_ptr<ClientProxyAPI> createProxy(const Value& param) = 0;
    };

    std::shared_ptr<BrokerFactoryAPI> nullBrokerFactory();
}
