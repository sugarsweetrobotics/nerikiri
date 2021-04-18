#include "nerikiri/ec.h"

using namespace nerikiri;

extern "C" {
    NK_OPERATION void* createOneShotEC();
};


class OneShotEC : public ExecutionContextBase {
public:
    OneShotEC() : ExecutionContextBase("OneShotEC") {}

public:
    virtual bool onStarted() override {
        svc();
        return false;
    }
};

void* createOneShotEC() {
    return new ECFactory<OneShotEC>("OneShotECFactory");
}
