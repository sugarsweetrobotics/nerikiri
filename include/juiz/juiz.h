#pragma once

#ifdef WIN32

#ifdef nerikiri_EXPORTS
#define NK_API __declspec(dllexport)
#else
#define NK_API __declspec(dllimport)
#endif

#ifdef _WINDLL
#define NK_OPERATION __declspec(dllexport)
#else
#define NK_OPERATION __declspec(dllimport)
#endif

#else

#define NK_API
#define NK_OPERATION
#endif

#include <string>
using ClassName = std::string;
using TypeName = std::string;

//#include "nerikiri/process.h"
//#include "nerikiri/operation.h"
