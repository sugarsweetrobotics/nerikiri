
#include <memory>
#include <juiz/factory_api.h>

using namespace juiz;


/*
class NullFactory : public FactoryAPI {
public:
    NullFactory() : FactoryAPI("NullFactory", "Null", "null") {}
    virtual ~NullFactory() {}

    virtual std::shared_ptr<Object> create(const std::string& _fullName, const Value& info={}) const override {
        return nullObject<Object>();
    }
};

std::shared_ptr<FactoryAPI> juiz::nullFactory() {
    return std::make_shared<NullFactory>();
}
*/