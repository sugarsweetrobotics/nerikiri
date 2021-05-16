
#include <juiz/broker_api.h>


using namespace juiz;


class NullBrokerFactory : public BrokerFactoryAPI {
public:
    NullBrokerFactory() : BrokerFactoryAPI("NullBrokerFactory", "NullBroker", "null") {}
    virtual ~NullBrokerFactory() {}

public:
    virtual std::shared_ptr<BrokerAPI> create(const Value& param) override {
        logger::error("NullBrokerFactory::{}() failed. Object is null.", __func__, param);
        return nullBroker();
    }

    virtual std::shared_ptr<ClientProxyAPI> createProxy(const Value& param) override {
        logger::error("NullBrokerFactory::{}() failed. Object is null.", __func__, param);
        return nullBrokerProxy();
    }

};

std::shared_ptr<BrokerFactoryAPI> juiz::nullBrokerFactory() {
    return std::make_shared<NullBrokerFactory>();
}