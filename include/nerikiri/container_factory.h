#pragma once

#include <vector>
#include <memory>
#include <string>
#include <nerikiri/container.h>
#include <nerikiri/container_operation.h>
#include <nerikiri/container_factory_api.h>

namespace nerikiri {


    /**
     * ContainerFactoryテンプレートクラス
     */
    template<typename T>
    class ContainerFactory : public ContainerFactoryAPI {
    public:

        /**
         * コンストラクタ
         */
        ContainerFactory(): ContainerFactoryAPI("ContainerFactory", demangle(typeid(T).name()), demangle(typeid(T).name())) {}


        /**
         * 
         */
        virtual ~ContainerFactory() {}
    public:


        /**
         * 
         * 
         */
        virtual std::shared_ptr<ContainerAPI> create(const std::string& fullName, const Value& info={}) override { 
            logger::info("ContainerFactory<{}>::create(fullName={}) called.", typeName(), fullName);
            auto c = std::make_shared<Container<T>>(this, fullName);
            c->getPoseOperation_ = std::make_shared<ContainerGetPoseOperation>();
            c->getPoseOperation_->setOwner(c);
            c->setPoseOperation_ = std::make_shared<ContainerSetPoseOperation>();
            c->setPoseOperation_->setOwner(c);
            return c;
        }
    };

    /**
     * ContainerFactoryの生成．ユーザはこの関数を使ってContainerFactoryを定義，アクセスできる
     */
    template<typename T>
    void* containerFactory() {
        logger::info("nerikiri::containerFactory<{}> called.", demangle(typeid(T).name()));
        return new ContainerFactory<T>(); 
    }

}