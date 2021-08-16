
#include <juiz/container.h>

using namespace juiz;

namespace juiz {
    std::shared_ptr<OperationAPI> createOperation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}, const std::function<Value(const Value&)>& func = nullptr, const Value& info={}); 

    // std::shared_ptr<OperationAPI> containerOperationBase(const std::string& _typeName, const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {});
}


class ContainerOperationBase : public OperationAPI {
public:
    static constexpr char classNameString[] = "ContainerOperation"; 
protected:
    std::shared_ptr<OperationAPI> base_;
    std::shared_ptr<ContainerAPI> container_;
    const std::function<Value(const Value&)> function_;
    std::mutex mutex_;
    Value defaultArgs_;
    Value info_;
public:


    virtual Value setOwner(const std::shared_ptr<Object>& container) override { 
        container_ = std::dynamic_pointer_cast<ContainerAPI>(container); 
        if (!container_) {
            return Value::error(logger::error("ContainerOperationBase::setOwner(container) failed. Passed argument (info={}) can not be converted to ContainerAPI pointer.", container->info()));
        }
        auto _typeName = typeName();
        setTypeName(juiz::naming::join(container->fullName(), _typeName));
        base_ = createOperation(juiz::naming::join(container->fullName(), _typeName), fullName(), defaultArgs_, 
           [this](auto value) {
                return this->function_(value);
        }, info_);
        return container_->info();
    }


    const std::shared_ptr<Object> getOwner() const override {
        return container_;
    }

public:
    ContainerOperationBase(const std::string& _typeName, const std::string& _fullName, const Value& defaultArgs, const std::function<Value(const Value&)> func, const Value& info={})
        : OperationAPI("ContainerOperation", _typeName, _fullName), defaultArgs_(defaultArgs), function_(func), info_(info)
        {
            if (info_.isObjectValue()) {
                if (info_.hasKey("description") && info_.isStringValue()) {
                    setDescription(info_["description"].stringValue());
                }
            }
        }

    virtual ~ContainerOperationBase() {}

    virtual Value fullInfo() const override { 
        auto i = base_->fullInfo(); 
        i["ownerContainerFullName"] = container_->fullName();
        return i;
    }


    virtual Value call(const Value& value) override {
        return Value::error("ContainerOperationBase::call called. This function must be overriden, but not.");
    }

    /**
     * This function collects the value from inlets and calls the function inside the Operation,
     * but this does not execute the output side operation
     */
    virtual Value invoke() override { return base_->invoke(); }

    virtual Value execute() override { return base_->execute(); }

    virtual std::shared_ptr<OutletAPI> outlet() const override { return base_->outlet(); }

    virtual std::shared_ptr<InletAPI> inlet(const std::string& name) const override { return base_->inlet(name); }

    virtual std::vector<std::shared_ptr<InletAPI>> inlets() const override { return base_->inlets(); }

    
};



std::shared_ptr<OperationAPI> juiz::containerOperationBase(const std::string& _typeName, const std::string& _fullName, const Value& defaultArgs, const std::function<Value(const Value&)> func, const Value& info) {
    return std::make_shared<ContainerOperationBase>(_typeName, _fullName, defaultArgs, func, info);
}

