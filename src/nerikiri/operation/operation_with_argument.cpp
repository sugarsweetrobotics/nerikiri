
#include <nerikiri/functional.h>
#include <nerikiri/operation_api.h>
#include <nerikiri/operation_with_argument.h>

using namespace nerikiri;


class NK_API OperationWithArgument : public OperationAPI {
private:
    const Value argument_;
    const std::shared_ptr<OperationAPI> operation_;
public:
    OperationWithArgument(const std::shared_ptr<OperationAPI>& op, const Value& v) : OperationAPI("OperationWithArgument", op->typeName(), op->fullName()), operation_(op), argument_(v) {}
    virtual ~OperationWithArgument() {}

public:

    virtual Value info() const override {
        auto v = operation_->info();
        v["argument"] = argument_;
        return v;
    }

    virtual Value fullInfo() const override {
        auto v = operation_->fullInfo();
        v["argument"] = argument_;
        return v;
    }

    virtual Value call(const Value& value) override { return operation_->call(value); }

    virtual Value invoke() override { return operation_->invoke(); }

    virtual Value execute() override {
        argument_.const_object_for_each([this](auto key, auto value) {
            auto inlet = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(operation_->inlets(), [&key, value] (auto inlet) {
                return inlet->name() == key;
            });
            if (!inlet) {
                logger::error("OperationWithArgument::execute() failed. Argument named '{}' not found", key);
            } else {
                inlet.value()->put(value);
            }
            //operation_->putToArgument(key, value);
        });
        return operation_->execute();
    }

    
    virtual std::shared_ptr<OperationOutletAPI> outlet() const override { return operation_->outlet(); }

    virtual std::shared_ptr<OperationInletAPI> inlet(const std::string& name) const override {
        auto i = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(inlets(), [&name](auto i) { return i->name() == name; });
        if (i) return i.value();
        return nullOperationInlet();
    }

    virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override { return operation_->inlets(); }

    //virtual Value putToArgument(const std::string& key, const Value& value) override { return operation_->putToArgument(key, value); }
};


std::shared_ptr<OperationAPI> nerikiri::operationWithArgument(const std::shared_ptr<OperationAPI>& op, const Value& v) {
    return std::make_shared<OperationWithArgument>(op, v);
}