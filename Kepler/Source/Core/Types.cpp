#include "Types.h"
#include <spdlog/fmt/fmt.h>

#include <cstdio>
#include <random>
#include <type_traits>

namespace Kepler
{

	TString ConvertToAnsiString(const TWideString& WideString)
	{
		TString OutString;
		OutString.resize(WideString.length());
		CHECK(std::wcstombs(OutString.data(), WideString.data(), OutString.length()));
		return OutString;
	}

	TWideString ConvertToWideString(const TString& AnsiString)
	{
		TWideString OutString;
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

	id64::id64(const TString& HashableString)
		: Value(std::hash<TString>{}(HashableString))
	{
	}

}