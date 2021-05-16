#include <juiz/operation.h>

using namespace juiz;


namespace juiz {
    std::shared_ptr<OperationAPI> createOperation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}, const std::function<Value(const Value&)>& func = nullptr); 
}

class OperationFactory : public OperationFactoryAPI{
private:
    const Value defaultArgs_;
    const std::function<Value(const Value&)> function_;
public:
    OperationFactory(const Value& info, const std::function<Value(const Value&)>& func) : OperationFactory(Value::string(info.at("typeName")), info.at("defaultArg"), func){}
    OperationFactory(const std::string& typeName, const Value& defaultArgs, const std::function<Value(const Value&)>& func):
        OperationFactoryAPI(typeName, typeName), defaultArgs_(defaultArgs), function_(func) {}
    virtual ~OperationFactory() {}

    virtual std::shared_ptr<Object> create(const std::string& _fullName, const Value& info={}) const override {
        return createOperation(typeName(), _fullName, defaultArgs_, function_); 
    }
};



void* juiz::operationFactory(const Value& info, std::function<Value(const Value&)>&& func) { return new OperationFactory(Value::string(info.at("typeName")), info.at("defaultArg"), func); }