#include <nerikiri/ec_proxy.h>
#include <nerikiri/functional.h>


using namespace nerikiri;

/*
class ECStateStartProxy : public ECStateAPI {
private:
    const std::shared_ptr<BrokerProxyAPI> broker_;
    const std::string fullName_;
public:
    ECStateStartProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName) : ECStateAPI("ECStateStartProxy", "Proxy", fullName), broker_(broker), fullName_(fullName) {}

    virtual ~ECStateStartProxy() {}

public:
    virtual Value activate() {
        return broker_->ec()->activateStart(fullName_);
    }
};
*/

/*
class ECStateStopProxy : public ECStateAPI {
private:
    const std::shared_ptr<BrokerProxyAPI> broker_;
    const std::string fullName_;
public:
    ECStateStopProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName) : ECStateAPI("ECStateStopProxy", "Proxy", fullName), broker_(broker), fullName_(fullName) {}

    virtual ~ECStateStopProxy() {}

public:
    virtual Value activate() {
        return broker_->ec()->activateStop(fullName_);
    }
};
*/

/*
std::shared_ptr<ECStateAPI> nerikiri::ecStateStartProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName) {
    return std::make_shared<ECStateStartProxy>(broker, fullName);
}

std::shared_ptr<ECStateAPI> nerikiri::ecStateStopProxy(const std::shared_ptr<BrokerProxyAPI>& broker, const std::string& fullName) {
    return std::make_shared<ECStateStopProxy>(broker, fullName);
}
*/