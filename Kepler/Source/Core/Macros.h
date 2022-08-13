#pragma once
#include <cassert>
#include <cstdlib>
#include <cstdio>

#ifdef USE_ASSERT
# define CHECK(x) do { if(!(x)) { fprintf(stderr, "Assertion failed: '%s' in file %s on line %d\n", #x, __FILE__, __LINE__); abort(); } } while(false)
#else
# define CHECK(x) x
#endif

#define CHECKED(x) [](auto&& arg) { CHECK(!!(arg)); return arg; }(x)
#define BIT(x) (1 << (x))