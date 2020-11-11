#include "nerikiri/ec.h"

using namespace nerikiri;

extern "C" {
    NK_OPERATION void* createOneShotEC();
};


class OneShotEC : public ExecutionContextBase {
public:
    OneShotEC(const Value& info) : ExecutionContextBase("OneShotEC", Value::string(info.at("fullName"))) {}

public:
    virtual bool onStarted() override {
        svc();
        return false;
    }
};

void* createOneShotEC() {
    return new ECFactory<OneShotEC>("OneShotECFactory");
}
