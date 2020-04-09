#pragma once

#include <memory>
#include "nerikiri/value.h"

namespace nerikiri {


    class DLLProxy {
    private:
        Value info_;
        std::string m_name;
        
        int m_mode;
        int m_closeflag;
        void* handle_;
        int m_error;
    public:
      DLLProxy(const Value& info);

      DLLProxy(const std::string& filename);
      ~DLLProxy();

    public:

      std::function<void*()> functionSymbol(const std::string& name);
    };

    std::shared_ptr<DLLProxy> createDLLProxy(const Value& info);

    std::shared_ptr<DLLProxy> createDLLProxy(const std::string& filename);

}