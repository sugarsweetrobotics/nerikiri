#include <nerikiri/operation_factory_api.h>

using namespace nerikiri;

/* 
class NullOperationFactory : public OperationFactoryAPI {
public:
    NullOperationFactory() : OperationFactoryAPI("NullOperation", "null") {}
    virtual ~NullOperationFactory() {}

    virtual std::shared_ptr<Object> create(const std::string& _fullName, const Value& info={}) const override {
        return nullOperation();
    }
};
*/
/*
using NullOperationFactory = NullFactory<OperationAPI>;

std::shared_ptr<OperationFactoryAPI> nerikiri::nullOperationFactory() {
    return std::make_shared<NullOperationFactory>();
}
*/