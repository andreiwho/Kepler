#pragma once
#ifdef SHARED_MODULE
#   ifdef WIN32
#       define CORE_API __declspec(dllexport)
#   else
#       error Unsupported platform
#   endif
#else
#   ifdef WIN32
#       define CORE_API __declspec(dllimport)
#   else
#       error Unsupported platform
#   endif
#endif