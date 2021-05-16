#pragma once 

#include <juiz/object.h>

namespace juiz {

    class InletAPI;
    class OutletAPI;

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

        ConnectionType type() const { return type_; }

        
        ConnectionType getType() const { return type_; }

        ConnectionAPI& setType(const ConnectionAPI::ConnectionType& type) {
            this->type_ = type;
            return *this;
        }

        bool isEvent() const { return type_ == EVENT; }
        bool isPull() const { return type_ == PULL; }
        bool isPush() const { return type_ == PUSH; }

        virtual std::shared_ptr<InletAPI> inlet() const =0;

        virtual std::shared_ptr<OutletAPI> outlet() const =0;

        virtual Value pull() =0;

        virtual Value put(const Value& value) =0;

    };

    ConnectionAPI::ConnectionType connectionType(const std::string& str);

    std::string toString(const ConnectionAPI::ConnectionType& typ);

    std::shared_ptr<ConnectionAPI> nullConnection();
}

// #include <juiz/null_connection.h>