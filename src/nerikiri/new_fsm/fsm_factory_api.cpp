#include <nerikiri/fsm_factory_api.h>

using namespace nerikiri;
/* 
class NullFSMFactory : public FSMFactoryAPI {
public:

    NullFSMFactory() : FSMFactoryAPI("NullFSMFactory", "NullFSMFactory", "null") {}
    virtual ~NullFSMFactory() {}

public:
    virtual std::shared_ptr<FSMAPI> create(const std::string& _fullName, const Value& extInfo) const override {
        logger::error("NullFSMFactory::{}() failed. FSMFactory is null.", __FUNCTION__);
        return nullFSM();
    }
};

std::shared_ptr<FSMFactoryAPI> nerikiri::nullFSMFactory() {
    return std::make_shared<NullFSMFactory>();
}
*/