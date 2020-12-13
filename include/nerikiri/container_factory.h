#pragma once

#include <vector>
#include <memory>
#include <string>
#include <nerikiri/container.h>
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
        ContainerFactory(const std::string& fullName): ContainerFactoryAPI("ContainerFactory", demangle(typeid(T).name()), fullName) {}


        /**
         * 
         */
        virtual ~ContainerFactory() {}
    public:


        /**
         * 
         * 
         */
        virtual std::shared_ptr<ContainerAPI> create(const std::string& fullName) override { 
            logger::info("ContainerFactory<{}>::create(fullName={}) called.", typeName(), fullName);
            return std::make_shared<Container<T>>(this, fullName);
        }
    };

    /**
     * ContainerFactoryの生成．ユーザはこの関数を使ってContainerFactoryを定義，アクセスできる
     */
    template<typename T>
    void* containerFactory() {
        logger::info("nerikiri::containerFactory<{}> called.", demangle(typeid(T).name()));
        return new ContainerFactory<T>("containerFactory" + demangle(typeid(T).name())); 
    }

}