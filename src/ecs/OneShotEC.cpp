#include "nerikiri/ec.h"

using namespace nerikiri;

extern "C" {
    void* createOneShotEC();
};


class OneShotEC : public ExecutionContext {
private:
public:
    OneShotEC(const Value& info) : ExecutionContext(info) {}
    virtual ~OneShotEC() {}

public:
    virtual bool onStarted() override {
        svc();
        return false;
    }

    virtual bool onStopping() override { 
        return true;
    }
};

class OneShotECFactory : public ExecutionContextFactory {
public:
    OneShotECFactory() {}
    virtual ~OneShotECFactory() {}
public:
    virtual std::shared_ptr<ExecutionContext> create(const Value& arg) const {
        return std::shared_ptr<ExecutionContext>(new OneShotEC(arg));
    };

    virtual std::string typeName() const { 
        return "OneShotEC";
    }
};

void* createOneShotEC() {
    return new OneShotECFactory();
}
