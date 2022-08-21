#pragma once
#include <cassert>
#include <cstdlib>
#include <cstdio>

#include "Exception.h"
#include <spdlog/fmt/fmt.h>

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

#ifdef USE_ASSERT
# define CHECK_NOTHROW(x) do { if(!(x)) { fprintf(stderr, "Assertion failed: '%s' in file %s on line %d\n", #x, __FILE__, __LINE__); exit(EXIT_FAILURE); } } while(false)
# define CHECK(x) do { if(!(x)) { throw Kepler::TException(fmt::format("Assertion failed: '{}' \nin file {} \non line {}\n", #x, __FILE__, __LINE__), "CHECK FAILURE"); } } while(false)
# define CRASH() DEBUG_BREAK; CHECK(false)
# define CRASHMSG(msg) DEBUG_BREAK; CHECKMSG(false, msg)
# define CHECKMSG(x, msg) do { if(!(x)) { throw Kepler::TException(fmt::format("Assertion failed: '{}' \nin file {} \non line {}\n", msg, __FILE__, __LINE__), "CHECK FAILURE"); } } while(false)
# define CHECKED(x) [](auto&& arg) { CHECK(!!(arg)); return arg; }(x)
#else
# define CHECK(x) x
# define CHECK_NOTHROW(x) x
# define CHECKMSG(x, msg) x
# define CHECKED(x) x
#endif

#define BIT(x) (1 << (x))