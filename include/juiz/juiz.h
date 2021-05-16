#pragma once

#ifdef WIN32

#ifdef juiz_EXPORTS
#define NK_API __declspec(dllexport)
#else
#define NK_API __declspec(dllimport)
#endif

#ifdef _WINDLL
#define JUIZ_OPERATION __declspec(dllexport)
#else
#define JUIZ_OPERATION __declspec(dllimport)
#endif

#else

#define NK_API
#define JUIZ_OPERATION
#endif

#include <string>
using ClassName = std::string;
using TypeName = std::string;

//#include "juiz/process.h"
//#include "juiz/operation.h"
