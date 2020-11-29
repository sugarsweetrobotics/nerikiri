#include <nerikiri/ec_factory_api.h>



using namespace nerikiri;


class NullExecutionContextFactory : public ExecutionContextFactoryAPI{
public:
    NullExecutionContextFactory() :
        ExecutionContextFactoryAPI("NullExecutionContextFactory", "null") {}
    virtual ~NullExecutionContextFactory() {}

    virtual std::string executionContextTypeFullName() const override  { return "NullExecutionContext"; }

    virtual std::shared_ptr<ExecutionContextAPI> create(const std::string& fullName) const override{
        logger::warn("NullExecutionContextFactory::create() called. ExecutionContextFactory is null.");
        return nullEC();
    }
};

std::shared_ptr<ExecutionContextFactoryAPI> nerikiri::nullECFactory() {
    return std::make_shared<NullExecutionContextFactory>();
}