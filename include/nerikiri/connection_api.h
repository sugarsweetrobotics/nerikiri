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
            EVENT,
            UNKNOWN
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

        virtual std::shared_ptr<OperationInletAPI> inlet() const =0;

        virtual std::shared_ptr<OperationOutletAPI> outlet() const =0;

        virtual Value pull() =0;

        virtual Value put(const Value& value) =0;

    };

    inline ConnectionAPI::ConnectionType connectionType(const std::string& str) {
        if (str == "PULL") return ConnectionAPI::ConnectionType::PULL;
        else if (str == "PUSH") return ConnectionAPI::ConnectionType::PUSH;
        else if (str == "EVENT") return ConnectionAPI::ConnectionType::EVENT;
        return ConnectionAPI::ConnectionType::UNKNOWN;
    };

    inline std::string toString(const ConnectionAPI::ConnectionType& typ) {
        if (typ == ConnectionAPI::ConnectionType::PULL) return "PULL";
        else if (typ == ConnectionAPI::ConnectionType::PUSH) return "PUSH";
        else if (typ == ConnectionAPI::ConnectionType::EVENT) return "EVENT";
        return "UNKNOWN";
    };

    std::shared_ptr<ConnectionAPI> nullConnection();
}

// #include <nerikiri/null_connection.h>