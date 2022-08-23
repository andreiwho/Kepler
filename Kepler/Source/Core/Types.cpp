#include "Types.h"
#include <spdlog/fmt/fmt.h>

#include <cstdio>
#include <random>
#include <type_traits>

namespace Kepler
{

	std::string ConvertToAnsiString(const std::wstring& WideString)
	{
		std::string OutString;
		OutString.resize(WideString.length());
		CHECK(std::wcstombs(OutString.data(), WideString.data(), OutString.length()));
		return OutString;
	}

	std::wstring ConvertToWideString(const std::string& AnsiString)
	{
		std::wstring OutString;
		OutString.resize(AnsiString.length());
		CHECK(std::mbstowcs(OutString.data(), AnsiString.data(), OutString.length()));
		return OutString;
	}

	//////////////////////////////////////////////////////////////////////////
	// Generate random 64bit id
	namespace
	{
		static u64 Types_RandomU64()
		{
			static std::random_device Dev{};
			static std::mt19937_64 RandomGenerator(Dev());
			static std::uniform_int_distribution<u64> Distribution{};
			return Distribution(RandomGenerator);
		}
	}

	id64::id64()
		:	Value(Types_RandomU64())
	{
	}

	id64::id64(const std::string& HashableString)
		: Value(std::hash<std::string>{}(HashableString))
	{
	}

}