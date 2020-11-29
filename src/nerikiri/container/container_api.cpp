#include <nerikiri/container_api.h>


using namespace nerikiri;


class NullContainer : public ContainerAPI {
public:

    NullContainer() : ContainerAPI("NullContainer", "NullContainer", "null") {}
    
    virtual ~NullContainer() {}


    virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override {
        logger::warn("NullContainer::operations() failed. Container is null.");
        return {};
    }

    virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const override {
        logger::warn("NullContainer::operation(std::string& const) failed. Container is null.");
        return nullOperation();;
    }
        
    virtual Value addOperation(const std::shared_ptr<OperationAPI>& operation) override {
        return Value::error(logger::warn("NullContainer::{}(std::string& const) failed. Container is null.", __func__));
    }

    virtual Value deleteOperation(const std::string& fullName) override {
        return Value::error(logger::warn("NullContainer::{}(std::string& const) failed. Container is null.", __func__));
    }

};

std::shared_ptr<ContainerAPI> nerikiri::nullContainer() {
    return std::make_shared<NullContainer>();
}