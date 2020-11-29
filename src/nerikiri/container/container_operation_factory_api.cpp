#include <nerikiri/container_operation_factory_api.h>

using namespace nerikiri;

class NullContainerOperationFactory : public ContainerOperationFactoryAPI {
public:
    virtual std::string containerTypeFullName() const override { return "NullContainer"; }
    virtual std::string operationTypeFullName() const override { return "NullOperation"; }
public:
    NullContainerOperationFactory() : ContainerOperationFactoryAPI("NullContainerOperationFactory", "NullOperation", "null") {}
    virtual ~NullContainerOperationFactory() {}

    virtual std::shared_ptr<OperationAPI> create(const std::shared_ptr<ContainerAPI>& container, const std::string& _fullName) const override {
        return nullOperation();
    }
};

std::shared_ptr<ContainerOperationFactoryAPI> nerikiri::nullContainerOperationFactory() {
    return std::make_shared<NullContainerOperationFactory>();
}