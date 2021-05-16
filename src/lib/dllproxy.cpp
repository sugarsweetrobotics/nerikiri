#include <juiz/logger.h>
#include <juiz/utils/dllproxy.h>
#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

using namespace juiz;

DLLProxy::DLLProxy(const Value& info):info_(info) {
    auto name = info.at("typeName").stringValue();
#ifdef WIN32
    dll_name_ = name + ".dll";
    if (!(handle_ = ::LoadLibraryEx(dll_name_.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH))) {
#else
    dll_name_ = name + ".dylib";
    if (!(handle_ = ::dlopen(dll_name_.c_str(), RTLD_LAZY))) {
#endif
        logger::debug("DLLProxy::DLLProxy failed. Con not open file({})", dll_name_);
    }
}

DLLProxy::DLLProxy(std::string path, const std::string& name): info_({{"typeName", name}}) {
    if (path.rfind("/") != path.length()) path = path + "/";
#ifdef WIN32
    dll_name_ = path + name + ".dll";
    if (!(handle_ = ::LoadLibraryEx(dll_name_.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH))) {
#else
    dll_name_ = path + name + ".dylib";
    if (!(handle_ = ::dlopen(dll_name_.c_str(), RTLD_LAZY))) {
#endif
     logger::debug("DLLProxy::DLLProxy failed. Con not open file({})", dll_name_);
    }
}

DLLProxy::~DLLProxy() {
    if (handle_) {
#if WIN32
        ::FreeLibrary(handle_);
#else
        ::dlclose(handle_); handle_ = nullptr;
#endif
    }
}

std::function<void*()> DLLProxy::functionSymbol(const std::string& name) {
    if (!handle_) return nullptr;
#if WIN32
    return reinterpret_cast<void* (*)()>(::GetProcAddress(handle_, name.c_str()) );

#else
    return reinterpret_cast<void* (*)()>(::dlsym(handle_, name.c_str()));
#endif
 }


std::shared_ptr<DLLProxy> juiz::createDLLProxy(const Value& info) {
    return std::make_shared<DLLProxy>(info);
}

std::shared_ptr<DLLProxy> juiz::createDLLProxy(const std::string& path, const std::string& name) {
    return std::make_shared<DLLProxy>(path, name);
}