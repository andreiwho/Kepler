#pragma once
#include "Macros.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <atomic>
#include <stdexcept>
#include <xhash>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
	using TString = std::string;
	using TWideString = std::wstring;
	using TPath = std::filesystem::path;

	using CStr = const char*;
	using WStr = const wchar_t*;

	TString ConvertToAnsiString(const TWideString& wide);
	TWideString ConvertToWideString(const TString& ansi);

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

	// Math types
	using float2 = glm::vec2;
	using float3 = glm::vec3;
	using float4 = glm::vec4;

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
}

namespace std
{
	template<>
	struct hash<ke::id64>
	{
		[[nodiscard]] size_t operator()(const ke::id64& id) const noexcept { return id.Value; }
	};
}