#pragma once 



#include <nerikiri/connection_api.h>


namespace nerikiri {
    class NullConnection : public ConnectionAPI {
    public:
        NullConnection();
    public:
        virtual ~NullConnection();

    public:
        virtual Value put(const Value& v) override;

        virtual Value pull() override;

        virtual std::shared_ptr<OperationInletAPI> inlet() const override;

        virtual std::shared_ptr<OperationOutletAPI> outlet() const override;
    };
}