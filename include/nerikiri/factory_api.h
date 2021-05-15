#pragma once

#include <string>
#include <memory>
#include <nerikiri/object.h>

namespace nerikiri {

    class FactoryAPI : public Object {
    private:
    public:
        FactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}
        virtual ~FactoryAPI() {}

        virtual std::shared_ptr<Object> create(const std::string& _fullName, const Value& info={}) const = 0;
        
        template<typename T>
        std::shared_ptr<T> create(const std::string& _fullName, const Value& info={}) const { 
            auto p = std::dynamic_pointer_cast<T>(create(_fullName, info));
            if (p) return p;
            return nullObject<T>();
        }
    };

    template<class Cls>
    class FactoryBase : public FactoryAPI {
    public:
        FactoryBase(const std::string& typeName, const std::string& fullName) : FactoryAPI(factoryClsNameString<Cls>(), typeName, fullName) {}
        virtual ~FactoryBase() {}
    };


    template<class Cls>
    class NullFactory : public FactoryBase<Cls> {
    public:
        NullFactory() : FactoryBase<Cls>(nullClsNameString<Cls>(), "null") { this->setClassName(nullFactoryClsNameString<Cls>()); }
        virtual ~NullFactory() {}

        virtual std::shared_ptr<Object> create(const std::string& _fullName, const Value& info={}) const override {
            return nullObject<Cls>();
        }
    };

    inline std::shared_ptr<FactoryAPI> nullFactory() { return std::make_shared<NullFactory<Object>>(); }

    template<>
    inline std::shared_ptr<FactoryAPI> nullObject<FactoryAPI>() { return nullFactory(); }

}