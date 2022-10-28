#include "Types.h"
#include <spdlog/fmt/fmt.h>

#include <cstdio>
#include <random>
#include <type_traits>

namespace ke
{

	String ConvertToAnsiString(const TWideString& wide)
	{
		String outStr;
		outStr.resize(wide.length());
		CHECK(std::wcstombs(outStr.data(), wide.data(), outStr.length()));
		return outStr;
	}

	TWideString ConvertToWideString(const String& ansi)
	{
		TWideString outStr;
		outStr.resize(ansi.length());
		CHECK(std::mbstowcs(outStr.data(), ansi.data(), outStr.length()));
		return outStr;
	}

	//////////////////////////////////////////////////////////////////////////
	// Generate random 64bit id
	namespace
	{
		static u64 Types_RandomU64()
		{
			static std::random_device device{};
			static std::mt19937_64 gen(device());
			static std::uniform_int_distribution<u64> dist{};
			return dist(gen);
		}
	}

	id64::id64()
		:	Value(Types_RandomU64())
	{
	}
}