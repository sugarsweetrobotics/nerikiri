#include <juiz/operation.h>

using namespace juiz;


namespace juiz {
    std::shared_ptr<OperationAPI> createOperation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}, const std::function<Value(const Value&)>& func = nullptr, const Value& info={}); 
}

class OperationFactory : public OperationFactoryAPI{
private:
    const Value defaultArgs_;
    const std::function<Value(const Value&)> function_;
    const Value info_;
public:
    OperationFactory(const Value& info, const std::function<Value(const Value&)>& func) : OperationFactory(Value::string(info.at("typeName")), info.at("defaultArg"), func, info) {}
    OperationFactory(const std::string& typeName, const Value& defaultArgs, const std::function<Value(const Value&)>& func, const Value& info):
        OperationFactoryAPI(typeName, typeName), defaultArgs_(defaultArgs), function_(func), info_(info) {}
    virtual ~OperationFactory() {}

    virtual std::shared_ptr<Object> create(const std::string& _fullName, const Value& info={}) const override {
        return createOperation(typeName(), _fullName, defaultArgs_, function_, Value::merge(info, info_)); 
    }
};



void* juiz::operationFactory(const Value& info, std::function<Value(const Value&)>&& func) { return new OperationFactory(Value::string(info.at("typeName")), info.at("defaultArg"), func, info); }