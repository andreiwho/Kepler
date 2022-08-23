#pragma once
#include "Macros.h"

#include <cstdint>
#include <string>
#include <atomic>
#include <stdexcept>
#include <xhash>

namespace Kepler
{
	using u8 = uint8_t;
	using ubyte = u8;
	using i8 = int8_t;
	using ibyte = i8;

	using u16 = uint16_t;
	using i16 = int16_t;

	using u32 = uint32_t;
	using i32 = int32_t;

	using u64 = uint64_t;
	using i64 = int64_t;

	using usize = u64;
	using isize = i64;

	template<typename T> using TAtomic = std::atomic<T>;
	using TString = std::string;
	using TWideString = std::wstring;

	TString ConvertToAnsiString(const TWideString& WideString);
	TWideString ConvertToWideString(const TString& AnsiString);

	// a 64 bit identifier, which claims to be unique
	struct id64
	{
		id64();
		id64(u64 InValue) : Value(InValue) {}
		id64(const TString& HashableString);
		id64(const id64& Other) noexcept { Value = Other.Value; }
		id64& operator=(const id64& Other) noexcept { Value = Other.Value; return *this; }

		u64 Value;
		inline operator u64() const { return Value; }
	};
}

namespace std
{
	template<>
	struct hash<Kepler::id64>
	{
		[[nodiscard]] size_t operator()(const Kepler::id64& Id) const noexcept { return Id.Value; }
	};
}