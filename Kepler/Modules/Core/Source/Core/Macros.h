#pragma once
#include "Build.h"
#include <cassert>
#include <cstdlib>
#include <cstdio>

#include "Exception.h"
#include <spdlog/fmt/fmt.h>

#ifdef ENABLE_PROFILING
# include "optick.h"
#endif

#ifdef USE_ASSERT
# if PLATFORM_DESKTOP
#  ifdef WIN32
#   define DEBUG_BREAK __debugbreak()
#  else
#   define DEBUG_BREAK 
#  endif
# else
#  define DEBUG_BREAK
# endif
#endif

#define KEPLER_DEPRECATED
#define KEPLER_DEPRECATED_MSG(...) [[deprecated(__VA_ARGS__)]]

#ifdef USE_ASSERT
# define CHECK_NOTHROW(x) do { if(!(x)) { fprintf(stderr, "Assertion failed: '%s' in file %s on line %d\n", #x, __FILE__, __LINE__); exit(EXIT_FAILURE); } } while(false)
# define CHECK(x) do { if(!(x)) { DEBUG_BREAK; throw ke::TException(fmt::format("Assertion failed: '{}' \nin file {} \non line {}\n", #x, __FILE__, __LINE__), "CHECK FAILURE"); } } while(false)
# define CRASH() DEBUG_BREAK; CHECK(false)
# define CRASHMSG(msg) DEBUG_BREAK; CHECKMSG(false, msg)
# define CHECKMSG(x, msg) do { if(!(x)) { throw ke::TException(fmt::format("Assertion failed: '{}' \nin file {} \non line {}\n", msg, __FILE__, __LINE__), "CHECK FAILURE"); } } while(false)
# define CHECKED(x) [](auto&& arg) { CHECK(!!(arg)); return arg; }(x)
#else
# define CHECK(x) (void)(x)
# define CHECK_NOTHROW(x) (void)(x)
# define CHECKMSG(x, msg) (void)(x)
# define CHECKED(x) (x)
# define CRASH() throw ke::TException("Exception raised")
# define CRASHMSG(x) throw ke::TException(x)
#endif

#define BIT(x) (1 << (x))

#ifdef ENABLE_PROFILING
# define KEPLER_PROFILE_FRAME(Name) OPTICK_FRAME((Name))
# define KEPLER_PROFILE_SCOPE(...) OPTICK_EVENT(__VA_ARGS__) 
# define KEPLER_PROFILE_INIT_THREAD(Name) OPTICK_THREAD((Name)) 
#else
# define KEPLER_PROFILE_FRAME(...)
# define KEPLER_PROFILE_SCOPE(...)
# define KEPLER_PROFILE_INIT_THREAD(...)
#endif

// Macros for KEReflector
#define reflected

#define kmeta(...)

// This is a special specifier, to note that this function is going to be filled in by the KEReflector
// Can be used only once per project
// Special class MUST have FillDatabaseEntries method
#define holding_reflection_data