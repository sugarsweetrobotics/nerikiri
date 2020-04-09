#include "nerikiri/dllproxy.h"
#include "nerikiri/logger.h"
#include <dlfcn.h>


using namespace nerikiri;

DLLProxy::DLLProxy(const Value& info):info_(info) {
    auto name = info.at("name").stringValue();
    int open_mode = RTLD_LAZY;
    auto dll_name = "lib" + name + ".dylib";
    if (!(handle_ = ::dlopen(dll_name.c_str(), open_mode))) {
        logger::error("DLLProxy::DLLProxy failed. Can not open file ({})", dll_name);
    }
}

DLLProxy::DLLProxy(const std::string& filename): info_({{"name", filename}}) {
    auto name = filename;
    int open_mode = RTLD_LAZY;
    auto dll_name = "lib" + name + ".dylib";
    if (!(handle_ = ::dlopen(dll_name.c_str(), open_mode))) {
        logger::error("DLLProxy::DLLProxy failed. Can not open file ({})", dll_name);
    }
}

DLLProxy::~DLLProxy() {
    if (handle_) {
        ::dlclose(handle_); handle_ = nullptr;
    }
}

std::function<void*()> DLLProxy::functionSymbol(const std::string& name) {
    if (!handle_) return nullptr;
    using LPVOIDFUNC = void*(*)();
    return reinterpret_cast<LPVOIDFUNC>(::dlsym(handle_, name.c_str()));
 }


std::shared_ptr<DLLProxy> nerikiri::createDLLProxy(const Value& info) {
    return std::make_shared<DLLProxy>(info);
}
std::shared_ptr<DLLProxy> nerikiri::createDLLProxy(const std::string& filename) {
    return std::make_shared<DLLProxy>(filename);
}