#pragma once
#include <cassert>

#define CHECKED(x) [](auto&& arg) { assert(!!(arg)); return arg; }(x)