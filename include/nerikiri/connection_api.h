#pragma once 

#include <nerikiri/object.h>
#include <nerikiri/operation_inlet_api.h>
#include <nerikiri/operation_outlet_api.h>

namespace nerikiri {

    class OperationInletAPI;
    class OperationOutletAPI;

    class ConnectionAPI : public Object {
    private:

    public:
        enum ConnectionType {
            PULL,
            PUSH,
            EVENT
        } type_;

    public:
        ConnectionAPI(const std::string& typeName, const std::string& fullName, const ConnectionType type) : Object(typeName, fullName), type_(type) {}

        virtual ~ConnectionAPI() {}


    public:
        ConnectionType getType() const { return type_; }

        ConnectionAPI& setType(const ConnectionAPI::ConnectionType& type) {
            this->type_ = type;
            return *this;
        }

        bool isEvent() const { return type_ == EVENT; }
        bool isPull() const { return type_ == PULL; }
        bool isPush() const { return type_ == PUSH; }

    };
}

#include <nerikiri/null_connection.h>