#include <nerikiri/operation_factory.h>

using namespace nerikiri;

class OperationFactory : public OperationFactoryAPI{
private:
    const Value defaultArgs_;
    //const std::string operationTypeFullName_;
    const std::function<Value(const Value&)> function_;
public:
    OperationFactory(const Value& info, const std::function<Value(const Value&)>& func) : OperationFactory(Value::string(info.at("typeName")), info.at("defaultArg"), func){}
    OperationFactory(const std::string& typeName, const Value& defaultArgs, const std::function<Value(const Value&)>& func):
        OperationFactoryAPI("OperationFactory", typeName, typeName + "0.opf"), defaultArgs_(defaultArgs), function_(func) {}
    virtual ~OperationFactory() {}

    virtual std::shared_ptr<OperationAPI> create(const std::string& _fullName) const {
        return createOperation(typeName(), _fullName, defaultArgs_, function_); 
    }
};



void* nerikiri::operationFactory(const Value& info, std::function<Value(const Value&)>&& func) { return new OperationFactory(Value::string(info.at("typeName")), info.at("defaultArg"), func); }