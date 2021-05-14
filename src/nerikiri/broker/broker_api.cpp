#include "nerikiri/broker_api.h"


using namespace nerikiri;



class NullBroker : public BrokerAPI {
private:
public:

public:
    NullBroker(): BrokerAPI("NullBroker", "NullBroker", "null") {}
    virtual ~NullBroker() {}

    virtual Value fullInfo() const override {
        return Value::error(logger::error("NullBroker::{}() failed. Object is null.", __func__));
    }

    virtual bool run(const std::shared_ptr<ClientProxyAPI>& coreBroker) override {
        logger::error("NullBroker::{}() failed. Object is null.", __func__);
        return false;
    }
    
    virtual void shutdown(const std::shared_ptr<ClientProxyAPI>& coreBroker) override {
        logger::error("NullBroker::{}() failed. Object is null.", __func__);
        return;
    }

    virtual bool isRunning() const override {
        logger::error("NullBroker::{}() failed. Object is null.", __func__);
        return false;
    }
};

std::shared_ptr<BrokerAPI> nerikiri::nullBroker() { return std::make_shared<NullBroker>(); }

