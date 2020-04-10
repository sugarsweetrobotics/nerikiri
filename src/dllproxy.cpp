#include "nerikiri/dllproxy.h"
#include "nerikiri/logger.h"
#include <dlfcn.h>


using namespace nerikiri;

DLLProxy::DLLProxy(const Value& info):info_(info) {
    auto name = info.at("name").stringValue();
    int open_mode = RTLD_LAZY;
    auto dll_name = "lib" + name + ".dylib";
    if (!(handle_ = ::dlopen(dll_name.c_str(), open_mode))) {
        logger::debug("DLLProxy::DLLProxy failed. Can not open file ({})", dll_name);
    }
}

DLLProxy::DLLProxy(std::string path, const std::string& name): info_({{"name", name}}) {
    int open_mode = RTLD_LAZY;
    if (path.rfind("/") != path.length()) path = path + "/";
    auto dll_name = path + "lib" + name + ".dylib";
    if (!(handle_ = ::dlopen(dll_name.c_str(), open_mode))) {
        logger::debug("DLLProxy::DLLProxy failed. Can not open file ({})", dll_name);
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
std::shared_ptr<DLLProxy> nerikiri::createDLLProxy(const std::string& path, const std::string& name) {
    return std::make_shared<DLLProxy>(path, name);
}