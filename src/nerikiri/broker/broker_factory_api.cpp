
#include <nerikiri/broker_factory_api.h>


using namespace nerikiri;


class NullBrokerFactory : public BrokerFactoryAPI {
public:
    NullBrokerFactory() : BrokerFactoryAPI("NullBrokerFactory", "NullBroker", "null") {}
    virtual ~NullBrokerFactory() {}

public:
    virtual std::shared_ptr<BrokerAPI> create(const Value& param) override {
        logger::error("NullBrokerFactory::{}() failed. Object is null.", __func__, param);
        return nullBroker();
    }

    virtual std::shared_ptr<BrokerProxyAPI> createProxy(const Value& param) override {
        logger::error("NullBrokerFactory::{}() failed. Object is null.", __func__, param);
        return nullBrokerProxy();
    }

};

std::shared_ptr<BrokerFactoryAPI> nerikiri::nullBrokerFactory() {
    return std::make_shared<NullBrokerFactory>();
}