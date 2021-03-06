#pragma once

#include <string>
#include <memory>
#ifdef WIN32
#include <Windows.h> // For HINSTANCE
#endif

#include <juiz/value.h>


namespace juiz {


    class DLLProxy {
    private:
        Value info_;
        std::string m_name;
        
        int m_mode;
        int m_closeflag;
#ifdef WIN32
        HINSTANCE handle_;
#else
        void* handle_;
#endif
        int m_error;
        std::string dll_name_;
    public:
      DLLProxy(const Value& info);

      DLLProxy(std::string path, const std::string& filename);
      ~DLLProxy();
      
      bool isNull() { return handle_ == 0; }
    public:

      std::function<void*()> functionSymbol(const std::string& name);
    };

    std::shared_ptr<DLLProxy> createDLLProxy(const Value& info);

    std::shared_ptr<DLLProxy> createDLLProxy(const std::string& path, const std::string& filename);

}