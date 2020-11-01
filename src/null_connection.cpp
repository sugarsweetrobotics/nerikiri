#include <nerikiri/null_connection.h>

using namespace nerikiri;

NullConnection::NullConnection() {}

NullConnection::~NullConnection() {}

Value NullConnection::put(const Value& v) override { 
    return Value::error(logger::error("NullConnection::{}({}) failed.  Connection is null.", __func__, v));
}
Value NullConnection::pull() override { 
    return Value::error(logger::error("NullConnection::{}() failed. Connection is null.", __func__));
}
std::shared_ptr<OperationInletAPI> NullConnection::inlet() const override { 
    logger::error("NullConnection::{}() failed. Connection is null.", __func__);
    return std::make_shared<NullOperationInlet>();
}std::shared_ptr<OperationOutletAPI> NullConnection::outlet() const override { 
    logger::error("NullConnection::{}() failed. Connection is null.", __func__);
    return std::make_shared<NullOperationOutlet>();
}
