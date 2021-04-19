#include <nerikiri/ec_factory_api.h>

using namespace nerikiri;

/**
 * 
 */
class NullExecutionContextFactory : public ExecutionContextFactoryAPI{
public:
    NullExecutionContextFactory() 
     : ExecutionContextFactoryAPI("NullExecutionContextFactory", "NullExecutionContext", "null") {}
    virtual ~NullExecutionContextFactory() {}

    virtual std::shared_ptr<ExecutionContextAPI> create(const Value& info) const override{
        logger::warn("NullExecutionContextFactory::create() called. ExecutionContextFactory is null.");
        return nullEC();
    }
};

std::shared_ptr<ExecutionContextFactoryAPI> nerikiri::nullECFactory() {
    return std::make_shared<NullExecutionContextFactory>();
}