#include "nerikiri/ec.h"

using namespace nerikiri;

extern "C" {
    NK_OPERATION void* createOneShotEC();
};


class OneShotEC : public ExecutionContext {
public:
    OneShotEC(const Value& info) : ExecutionContext(info) {}

public:
    virtual bool onStarted() override {
        svc();
        return false;
    }
};

void* createOneShotEC() {
    return new ECFactory<OneShotEC>();
}
