#pragma once

#include <vector>
#include <memory>
#include <string>
#include "nerikiri/container.h"

namespace nerikiri {


    /**
     * ContainerFactoryテンプレートクラス
     */
    template<typename T>
    class ContainerFactory : public ContainerFactoryBase {
    public:

        /**
         * コンストラクタ
         */
        ContainerFactory(const std::string& fullName): ContainerFactoryBase("ContainerFactory", demangle(typeid(T).name()), fullName) {}


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
            //auto i = info;
            //i["typeName"] = demangle(typeid(T).name());
            //auto c = std::shared_ptr<ContainerBase>(new Container<T>(this, i)); 
            return std::make_shared<Container<T>>(this, fullName);
//            return c;
        }
    };

    /**
     * ContainerFactoryの生成．ユーザはこの関数を使ってContainerFactoryを定義，アクセスできる
     */
    template<typename T>
    void* containerFactory() { return new ContainerFactory<T>("containerFactory" + demangle(typeid(T).name())); }

}