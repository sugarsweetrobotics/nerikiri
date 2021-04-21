#include <nerikiri/operation_factory_api.h>

using namespace nerikiri;


class NullOperationFactory : public OperationFactoryAPI {
public:
NullOperationFactory() : OperationFactoryAPI("NullOperationFactory", "NullOperation", "null") {}
virtual ~NullOperationFactory() {}

virtual std::shared_ptr<OperationAPI> create(const std::string& _fullName, const Value& info={}) const override {
    return nullOperation();
}
};

std::shared_ptr<OperationFactoryAPI> nerikiri::nullOperationFactory() {
    return std::make_shared<NullOperationFactory>();
}