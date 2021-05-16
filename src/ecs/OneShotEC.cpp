#include <juiz/ec.h>

using namespace juiz;

extern "C" {
    JUIZ_OPERATION void* createOneShotEC();
};


class OneShotEC : public ExecutionContextBase {
public:
    OneShotEC(const Value& val) : ExecutionContextBase("OneShotEC") {}

public:
    virtual bool onStarted() override {
        svc();
        return true;
    }
};

void* createOneShotEC() {
    return new ECFactory<OneShotEC>();
}
