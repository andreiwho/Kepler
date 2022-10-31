#pragma once
#include "Macros.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <atomic>
#include <stdexcept>
#include <xhash>
#include <string_view>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <optional>

namespace ke
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
	using String = std::string;
	using TWideString = std::wstring;
	using TPath = std::filesystem::path;
	using StringView = std::string_view;

	using CStr = const char*;
	using WStr = const wchar_t*;

	String ConvertToAnsiString(const TWideString& wide);
	TWideString ConvertToWideString(const String& ansi);

	// FNV-1a constexpr hashing functions 
	constexpr u64 Hash32(const char* str, usize n, uint32_t basis = UINT32_C(2166136261)) {
		if (n == 0)
		{
			return basis;
		}
		return Hash32(str + 1, n - 1, (basis ^ str[0]) * UINT32_C(16777619));
	}

	constexpr u64 Hash64(const char* str, usize n, uint64_t basis = UINT64_C(14695981039346656037)) {
		if (n == 0)
		{
			return basis;
		}
		return Hash64(str + 1, n - 1, (basis ^ str[0]) * UINT64_C(1099511628211));
	}

	template< usize N >
	constexpr u64 Hash32(const char(&s)[N]) {
		return Hash32(s, N - 1);
	}

	template< usize N >
	constexpr u64 Hash64(const char(&s)[N]) {
		return Hash64(s, N - 1);
	}
	
	// a 64 bit identifier, which claims to be unique
	struct id64
	{
		static constexpr u64 none = 0;

		id64();
		constexpr id64(u64 InValue) : Value(InValue) {}
		explicit constexpr id64(const String& str)
			:	Value(Hash64(str.c_str(), str.length()))
		{

		}

		constexpr id64(const id64& Other) noexcept { Value = Other.Value; }
		constexpr id64& operator=(const id64& Other) noexcept { Value = Other.Value; return *this; }
		String ToString() const { return std::to_string(Value); }

		u64 Value;
		inline constexpr operator u64() const { return Value; }
	};

	using uuid64 = id64;
	using typehash64 = id64;

	// Math types
	using float2 = glm::vec2;
	using float3 = glm::vec3;
	using float4 = glm::vec4;
	using color3 = float3;
	using color4 = float4;

	using int2 = glm::ivec2;
	using int3 = glm::ivec3;
	using int4 = glm::ivec4;

	using uint2 = glm::uvec2;
	using uint3 = glm::uvec3;
	using uint4 = glm::uvec4;

	using matrix3x3 = glm::mat3x3;
	using matrix3x4 = glm::mat3x4;
	using matrix4x3 = glm::mat4x3;
	using matrix4x4 = glm::mat4x4;
	using matrix = matrix4x4;

	template<typename T> using Option = std::optional<T>;

	enum class EBaseTypeId : u32
	{
		_u8,
		_u16,
		_u32,
		_u64,

		_i8,
		_i16,
		_i32,
		_i64,
		_usize,
		_isize,

		_bool,
		_float,
		_double,
		_String,

		_float2,
		_float3,
		_float4,

		_int2,
		_int3,
		_int4,

		_uint2,
		_uint3,
		_uint4,

		_matrix3x3,
		_matrix3x4,
		_matrix4x3,
		_matrix4x4,
		_matrix = _matrix4x4,
	};
}

namespace std
{
	template<>
	struct hash<ke::id64>
	{
		[[nodiscard]] size_t operator()(const ke::id64& id) const noexcept { return id.Value; }
	};
}