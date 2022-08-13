#pragma once
#include "Macros.h"

#include <cstdint>
#include <string>

namespace Kepler
{
	using ubyte = uint8_t;
	using ibyte = int8_t;

	using u16 = uint16_t;
	using i16 = int16_t;

	using u32 = uint32_t;
	using i32 = int32_t;

	using u64 = uint64_t;
	using i64 = int64_t;

	using usize = u64;
	using isize = i64;

	std::string ConvertToAnsiString(const std::wstring& WideString);
	std::wstring ConvertToWideString(const std::string& AnsiString);
}